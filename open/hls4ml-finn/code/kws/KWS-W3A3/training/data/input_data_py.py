# Copyright 2017 The TensorFlow Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ==============================================================================
"""Model definitions for simple speech recognition.

"""

import hashlib
import math
import os.path
import random
import re
import json
import numpy as np
import torch
import glob

import torchaudio
import torchaudio.transforms
from torchaudio.datasets import SPEECHCOMMANDS

from data.perturb import WhiteNoisePerturbation
from data.spectrogram_augmentation import SpecCutout, SpecAugment
from data.features import CropOrPadSpectrogramAugmentation
import os

import cProfile
import time

MAX_NUM_WAVS_PER_CLASS = 2 ** 27 - 1  # ~134M
RANDOM_SEED = 59185  # Different from CONFIG. MUST USE THIS


def read_wav_np(path, desidered_samples=None):
    wav, sr = torchaudio.load(path)

    assert sr == 16000, "Sample rate must be 16000 for samples from the Speech Commands dataset."
    if len(wav.shape) == 2:
        wav = wav[0, :]

    if desidered_samples is not None and len(wav) < desidered_samples:
        wav = np.pad(wav, (0, desidered_samples - len(wav)), constant_values=0)

    return sr, wav


def parse_item(line, manifest_file):
    item = json.loads(line)

    # Audio file
    if 'audio_filename' in item:
        item['audio_file'] = item.pop('audio_filename')
    elif 'audio_filepath' in item:
        item['audio_file'] = item.pop('audio_filepath')
    else:
        raise ValueError(
            f"Manifest file has invalid json line " f"structure: {line} without proper audio file key."
        )
    item['audio_file'] = os.path.expanduser(item['audio_file'])


    # Label.
    if 'command' in item:
        item['label'] = item.pop('command')
    elif 'target' in item:
        item['label'] = item.pop('target')
    elif 'label' in item:
        pass
    else:
        raise ValueError(f"Manifest file has invalid json line " f"structure: {line} without proper label key.")

    item = dict(
        audio_file=item['audio_file'],
        label=item['label'],
    )

    return item


def item_iter(manifests_files, parse_func):
    if isinstance(manifests_files, str):
        manifests_files = [manifests_files]

    if parse_func is None:
        parse_func = parse_item

    for manifest_file in manifests_files:
        with open(os.path.expanduser(manifest_file), 'r') as f:
            for line in f:
                item = parse_func(line, manifest_file)

                yield item


def which_set(filename, validation_percentage, testing_percentage):
    """Determines which data partition the file should belong to.

  We want to keep files in the same training, validation, or testing sets even
  if new ones are added over time. This makes it less likely that testing
  samples will accidentally be reused in training when long runs are restarted
  for example. To keep this stability, a hash of the filename is taken and used
  to determine which set it should belong to. This determination only depends on
  the name and the set proportions, so it won't change as other files are added.

  It's also useful to associate particular files as related (for example words
  spoken by the same person), so anything after '_nohash_' in a filename is
  ignored for set determination. This ensures that 'bobby_nohash_0.wav' and
  'bobby_nohash_1.wav' are always in the same set, for example.

  Args:
    filename: File path of the data sample.
    validation_percentage: How much of the data set to use for validation.
    testing_percentage: How much of the data set to use for testing.

  Returns:
    String, one of 'training', 'validation', or 'testing'.
  """
    base_name = os.path.basename(filename)
    # We want to ignore anything after '_nohash_' in the file name when
    # deciding which set to put a wav in, so the data set creator has a way of
    # grouping wavs that are close variations of each other.
    hash_name = re.sub(r'_nohash_.*$', '', base_name)
    # This looks a bit magical, but we need to decide whether this file should
    # go into the training, testing, or validation sets, and we want to keep
    # existing files in the same set even if more files are subsequently
    # added.
    # To do that, we need a stable way of deciding based on just the file name
    # itself, so we do a hash of that and then use that to generate a
    # probability value that we use to assign it.
    hash_name_hashed = hashlib.sha1(str.encode(hash_name)).hexdigest()
    percentage_hash = ((int(hash_name_hashed, 16) %
                        (MAX_NUM_WAVS_PER_CLASS + 1)) *
                       (100.0 / MAX_NUM_WAVS_PER_CLASS))
    if percentage_hash < validation_percentage:
        result = 'validation'
    elif percentage_hash < (testing_percentage + validation_percentage):
        result = 'testing'
    else:
        result = 'training'
    return result


class AudioProcessor(object):
    """Handles Partitioning, and preparing audio training data."""

    def __init__(self, args, profiling_prefix=None):
        self.args = args
        self.background_data = []
        self.word_to_index = {}
        self.wanted_words = args['wanted_words'].split(',')
        num_classes = len(self.wanted_words)

        # Get dataset from torchaudio
        sc_datset = SPEECHCOMMANDS(args['docker_dataset_dir'], url=args['speech_commands_version'], download=False)
        self.data_dir = sc_datset._path

        rebalance = args['rebalance'] if args['rebalance'] else False
        self.data_index = {'validation': [], 'testing': [], 'training': []}

        if num_classes < 35:
            self.prepare_data_index_red(args['silence_percentage'], args['unknown_percentage'],
                                        self.wanted_words, args['validation_percentage'],
                                        args['testing_percentage'], rebalance=rebalance)
        else:
            self.prepare_data_index_full(rebalance=rebalance)

        self.data_index['training'], self.data_index['validation'], self.data_index['testing'] = self.load_jsons()

        self.prepare_data_structure(self.wanted_words)
        self.prepare_background_data()

        if args['preprocess_config_perturb']:
            self.perturb = WhiteNoisePerturbation()
        else:
            self.perturb = None

        mel_kwargs = {}
        mel_kwargs['f_min'] = args['preprocess_config_f_min']
        mel_kwargs['f_max'] = None if args['preprocess_config_f_max'] == 'None' else args['preprocess_config_f_max']
        mel_kwargs['n_mels'] = args['preprocess_config_n_mels']

        mel_kwargs['n_fft'] = args['preprocess_config_n_fft']

        mel_kwargs['win_length'] = args['preprocess_config_window_size_samples']
        mel_kwargs['hop_length'] = args['preprocess_config_window_stride_samples']
        # ToDo: Update torchaudio and test if this parameter resolves
        #  the issue with the mismatching "preprocess_config_window_stride_samples" parameter
        #  between the current config and the TinyML config
        #mel_kwargs['center'] = False

        # Set window_fn. None defaults to torch.ones.
        mel_kwargs['window_fn'] = torch.hann_window

        # Use torchaudio's implementation of MFCCs as featurizer
        self.featurizer = torchaudio.transforms.MFCC(
            sample_rate=args['preprocess_config_sample_rate'],
            n_mfcc=args['preprocess_config_n_mfcc'],
            dct_type=2,
            norm="ortho",
            log_mels=True,
            melkwargs=mel_kwargs,
        )

        if args['preprocess_config_pad_spectrogram']:
            self.pad_spectrogram = CropOrPadSpectrogramAugmentation(args['preprocess_config_spectrogram_length'])
        else:
            self.pad_spectrogram = None

        if args['preprocess_config_spec_cutout']:
            self.spec_cutout = SpecCutout(rect_masks=5, rect_time=10, rect_freq=2)
        else:
            self.spec_cutout = None

        if args['preprocess_config_spec_augment']:
            self.spec_augment = SpecAugment(freq_masks=2, time_masks=2, freq_width=2, time_width=10)
        else:
            self.spec_augment = None

        self._profiling = False
        if profiling_prefix is not None:
            self._profiling = True
            self._profiling_prefix = profiling_prefix

    def prepare_data_index_red(self, silence_percentage, unknown_percentage,
                               wanted_words, validation_percentage,
                               testing_percentage, rebalance=False):
        """Prepares a list of the samples organized by set and label.

    The training loop needs a list of all the available data, organized by
    which partition it should belong to, and with ground truth labels attached.
    This function analyzes the folders below the `data_dir`, figures out the
    right
    labels for each file based on the name of the subdirectory it belongs to,
    and uses a stable hash to assign it to a data set partition.

    Args:
      silence_percentage: How much of the resulting data should be background.
      unknown_percentage: How much should be audio outside the wanted classes.
      wanted_words: Labels of the classes we want to be able to recognize.
      validation_percentage: How much of the data set to use for validation.
      testing_percentage: How much of the data set to use for testing.

    Returns:
      Dictionary containing a list of file information for each set partition,
      and a lookup map for each class to determine its numeric index.

    Raises:
      Exception: If expected files are not found.
    """
        # Make sure the shuffling and picking of unknowns is deterministic.
        random.seed(RANDOM_SEED)
        data_index = {'validation': [], 'testing': [], 'training': []}

        wanted_words_index = {}
        for index, wanted_word in enumerate(wanted_words):
            wanted_words_index[wanted_word] = index
        unknown_index = {'validation': [], 'testing': [], 'training': []}
        all_words = {}
        label_count = {}
        label_filepaths = {}
        # Look through all the subfolders to find audio samples
        labels = next(os.walk(self.data_dir))[1]
        for label in labels:
            full_label_path = os.path.join(self.data_dir, label)
            files = os.listdir(full_label_path)
            label = label.lower()
            if label == self.args['background_noise_dir_name']:
                continue
            all_words[label] = True
            for file in files:
                file_path = os.path.join(full_label_path, file)
                set_index = which_set(file_path, validation_percentage, testing_percentage)
                if label in wanted_words_index:
                    data_index[set_index].append((label, file_path))
                else:
                    unknown_index[set_index].append((label, file_path))

                if set_index == 'training' and label in wanted_words_index:
                    if label in label_count:
                        label_count[label] += 1
                    else:
                        label_count[label] = 1

                    if label in label_filepaths:
                        label_filepaths[label] += [(label, file_path)]
                    else:
                        label_filepaths[label] = [(label, file_path)]

        max_command = None
        max_count = -1
        for command, count in label_count.items():
            if count > max_count:
                max_count = count
                max_command = command

        if rebalance:
            data_index['training'] = self.rebalance_train(max_command, max_count, label_filepaths)

        if not all_words:
            raise Exception('No .wavs found')
        for index, wanted_word in enumerate(wanted_words):
            if wanted_word not in all_words:
                raise Exception('Expected to find ' + wanted_word +
                                ' in labels but only found ' +
                                ', '.join(all_words.keys()))
        # We need an arbitrary file to load as the input for the silence samples.
        # It's multiplied by zero later, so the content doesn't matter.
        silence_wav_path = data_index['training'][0][1]
        for set_index in ['validation', 'testing', 'training']:
            set_size = len(data_index[set_index])
            silence_size = int(math.ceil(set_size * silence_percentage / 100))
            print(set_index)
            print(set_size)
            for _ in range(silence_size):
                data_index[set_index].append((self.args['silence_label'], silence_wav_path))
            # Pick some unknowns to add to each partition of the data set.
            random.shuffle(unknown_index[set_index])

            unknown_size = int(math.ceil(set_size * unknown_percentage / 100))
            print(unknown_size)
            data_index[set_index].extend(unknown_index[set_index][:unknown_size])
        # Make sure the ordering is random.
        for set_index in ['validation', 'testing', 'training']:
            random.shuffle(data_index[set_index])


        manifests = [
            ('train_manifest.json', data_index['training']),
            ('validation_manifest.json', data_index['validation']),
            ('test_manifest.json', data_index['testing']),
        ]
        for manifest_filename, dataset in manifests:
            with open(os.path.join(self.data_dir, manifest_filename), 'w') as fout:
                for label, audio_path in dataset:
                    # duration = librosa.core.get_duration(filename=audio_path)

                    # Write the metadata to the manifest
                    metadata = {
                        "audio_filepath": audio_path,
                        # "duration": duration,
                        "command": label,
                    }
                    json.dump(metadata, fout)
                    fout.write('\n')
                    fout.flush()

    def prepare_data_structure(self, wanted_words):
        # Prepare the rest of the result data structure.
        all_words = {}
        wanted_words_index = {}
        for index, wanted_word in enumerate(wanted_words):
            wanted_words_index[wanted_word] = index

        self.words_list = [self.args['silence_label'], self.args['unknown_word_label']] + wanted_words
        labels = next(os.walk(self.data_dir))[1]
        for label in labels:
            label = label.lower()
            if label == self.args['background_noise_dir_name']:
                continue
            all_words[label] = True
        for word in all_words:
            if word in wanted_words_index:
                self.word_to_index[word] = wanted_words_index[word]
            else:
                self.word_to_index[word] = self.args['unknown_word_index']
        self.word_to_index[self.args['silence_label']] = self.args['silence_index']

    def prepare_data_index_full(self, rebalance=False):
        pattern = re.compile(r"(.+\/)?(\w+)\/([^_]+)_.+wav")
        all_files = glob.glob(os.path.join(self.data_dir, '*/*wav'))

        with open(os.path.join(self.data_dir, 'validation_list.txt'), 'r') as fin:
            validation_files = fin.readlines()

        valset = set()
        for entry in validation_files:
            r = re.match(pattern, entry)
            if r:
                valset.add(r.group(3))

        with open(os.path.join(self.data_dir, 'testing_list.txt'), 'r') as fin:
            testing_files = fin.readlines()

        testset = set()
        for entry in testing_files:
            r = re.match(pattern, entry)
            if r:
                testset.add(r.group(3))

        label_count = {}
        label_filepaths = {}

        train, val, test = [], [], []
        for entry in all_files:
            r = re.match(pattern, entry)
            if r:
                label, uid = r.group(2), r.group(3)
                if label == '_background_noise_':
                    continue

                sample = (label, entry)

                if uid not in valset and uid not in testset:
                    if label in label_count:
                        label_count[label] += 1
                    else:
                        label_count[label] = 1

                    if label in label_filepaths:
                        label_filepaths[label] += [sample]
                    else:
                        label_filepaths[label] = [sample]

                if uid in valset:
                    val.append(sample)
                elif uid in testset:
                    test.append(sample)
                else:
                    train.append(sample)

        max_command = None
        max_count = -1
        for command, count in label_count.items():
            if count > max_count:
                max_count = count
                max_command = command
        if rebalance:
            train = self.rebalance_train(max_command, max_count, label_filepaths)

        manifests = [
            ('train_manifest.json', train),
            ('validation_manifest.json', val),
            ('test_manifest.json', test),
        ]

        for manifest_filename, dataset in manifests:
            with open(os.path.join(self.data_dir, manifest_filename), 'w') as fout:
                for label, audio_path in dataset:
                    # duration = librosa.core.get_duration(filename=audio_path)

                    # Write the metadata to the manifest
                    metadata = {
                        "audio_filepath": audio_path,
                        # "duration": duration,
                        "command": label,
                    }
                    json.dump(metadata, fout)
                    fout.write('\n')
                    fout.flush()

            print(f"Finished construction of manifest : {manifest_filename}")


    def rebalance_train(self, max_command, max_count, label_filepaths):
        print(f"Command with maximum number of samples = {max_command} with {max_count} samples")
        print(f"Rebalancing dataset by duplicating classes with less than {max_count} samples...")

        for command, samples in label_filepaths.items():
            filepaths = [sample[1] for sample in samples]

            rng = np.random.RandomState(0)
            filepaths = np.asarray(filepaths)
            num_samples = len(filepaths)

            if num_samples < max_count:
                difference = max_count - num_samples
                duplication_ids = rng.choice(num_samples, difference, replace=True)

                filepaths = np.append(filepaths, filepaths[duplication_ids], axis=0)

                print(f"Extended class label {command} from {num_samples} samples to {len(filepaths)} samples")

                label_filepaths[command] = [(command, filepath) for filepath in filepaths]

        train = []
        for label, samples in label_filepaths.items():
            train.extend(samples)

        return train

    def load_jsons(self):
        train_list, validation_list, test_list = [], [], []
        manifests = [
            ('train_manifest.json', train_list),
            ('validation_manifest.json', validation_list),
            ('test_manifest.json', test_list),
        ]
        for file, mapping in manifests:
            fullpath = os.path.join(self.data_dir, file)
            # audio_files, durations, labels, offsets = [], [], [], []

            for item in item_iter(fullpath, parse_func=parse_item):
                mapping.append((item['audio_file'], item['label']))
            #     audio_files.append(item['audio_file'])
            #     labels.append(item['label'])
            # mapping['file'] = audio_files
            # mapping['label'] = labels
        return train_list, validation_list, test_list

    def prepare_background_data(self):
        """Searches a folder for background noise audio, and loads it into memory.

    It's expected that the background audio samples will be in a subdirectory
    named '_background_noise_' inside the 'data_dir' folder, as .wavs that match
    the sample rate of the training data, but can be much longer in duration.

    If the '_background_noise_' folder doesn't exist at all, this isn't an
    error, it's just taken to mean that no background noise augmentation should
    be used. If the folder does exist, but it's empty, that's treated as an
    error.

    Returns:
      List of raw PCM-encoded audio samples of background noise.

    Raises:
      Exception: If files aren't found in the folder.
    """

        background_dir = os.path.join(self.data_dir, self.args["background_noise_dir_name"])
        if not os.path.exists(background_dir):
            return self.background_data
        search_path = os.path.join(background_dir)
        wav_filenames = []
        for el in next(os.walk(search_path))[2]:
            if 'wav' in el:
                wav_filenames.append(os.path.join(search_path, el))
        # wav_filenames = next(os.walk(search_path))[2]

        for wav_filename in wav_filenames:
            sr, audio = read_wav_np(wav_filename)
            self.background_data.append(audio)

        if not self.background_data:
            raise Exception('No background wav files were found in ' + search_path)

    def prepare_processing_graph(self, wav_filename, foreground_volume, background_data,
                                 background_volume,
                                 time_shift_padding, time_shift_offset, mode):
        """Builds a TensorFlow graph to apply the input distortions.

    Creates a graph that loads a WAVE file, decodes it, scales the volume,
    shifts it in time, adds in background noise, calculates a spectrogram, and
    then builds an MFCC fingerprint from that.

    This must be called with an active TensorFlow session running, and it
    creates multiple placeholder inputs, and one output:

      - wav_filename_placeholder_: Filename of the WAV to load.
      - foreground_volume_placeholder_: How loud the main clip should be.
      - time_shift_padding_placeholder_: Where to pad the clip.
      - time_shift_offset_placeholder_: How much to move the clip in time.
      - background_data_placeholder_: PCM sample data for background noise.
      - background_volume_placeholder_: Loudness of mixed-in background.
      - mfcc_: Output 2D fingerprint of processed audio.

    Args:
      model_settings: Information about the current model being trained.
    """
        desired_samples = self.args['preprocess_config_desired_samples']
        sr, audio = read_wav_np(wav_filename, desidered_samples=desired_samples)
        audio = audio[: desired_samples]

        audio = np.expand_dims(audio, 1)
        scaled_foreground = audio * foreground_volume
        # Shift the sample's start position, and pad any gaps with zeros.
        padded_foreground = np.pad(
            scaled_foreground,
            time_shift_padding,
            mode='constant')
        sliced_foreground = padded_foreground[time_shift_offset[0]:desired_samples + time_shift_offset[0],
                            time_shift_offset[1]:]

        # Mix in background or white noise.
        if self.perturb is not None and mode == 'training':
            preprocessed_data = self.perturb.process(sliced_foreground.squeeze(1))
            preprocessed_data = np.expand_dims(preprocessed_data, 1)
        else:
            background_data = background_data
            background_volume = background_volume

            background_mul = background_data * background_volume
            background_add = sliced_foreground + background_mul

            background_clamp = np.clip(background_add, -1.0, 1.0)
            background_clamp = np.squeeze(background_clamp, 1)
            preprocessed_data = background_clamp

        with torch.no_grad():
            preprocessed_data = np.transpose(preprocessed_data)
            preprocessed_data = torch.tensor(preprocessed_data, dtype=torch.float)
            output = self.featurizer(preprocessed_data)

        if self.pad_spectrogram:
            image_len = output.shape[-1]
            pad_left = (128 - image_len) // 2
            pad_right = (128 - image_len) // 2

            if (128 - image_len) % 2 == 1:
                pad_right += 1

            output = torch.nn.functional.pad(output, [pad_left, pad_right], mode="constant", value=0)

        if self.spec_cutout is not None and mode == 'training':
            with torch.no_grad():
                output = self.spec_cutout(output)

        if self.spec_augment is not None and mode == 'training':
            with torch.no_grad():
                output = self.spec_augment(output)
        return output

    # def set_size(self, mode):
    #     """Calculates the number of samples in the dataset partition.
    #
    # Args:
    #   mode: Which partition, must be 'training', 'validation', or 'testing'.
    #
    # Returns:
    #   Number of samples in the partition.
    # """
    #     return len(self.data_index[mode])

    def len_dataset(self, mode):
        candidates = self.data_index[mode]
        return len(candidates)

    def get_data(self, offset, mode):
        if self._profiling:
            pr = cProfile.Profile()
            pr.enable()

        ret = self.get_data_intern(offset, mode)

        if self._profiling:
            pr.disable()
            pr.dump_stats(f"{self._profiling_prefix}get_data_{mode}_{time.time()}.prof")

        return ret

    def get_data_intern(self, offset, mode):
        """Gather samples from the data set, applying transformations as needed.

    When the mode is 'training', a random selection of samples will be returned,
    otherwise the first N clips in the partition will be used. This ensures that
    validation always uses the same samples, reducing noise in the metrics.

    Args:
      how_many: Desired number of samples to return. -1 means the entire
        contents of this partition.
      offset: Where to start when fetching deterministically.
      model_settings: Information about the current model being trained.
      background_frequency: How many clips will have background noise, 0.0 to
        1.0.
      background_volume_range: How loud the background noise will be.
      time_shift: How much to randomly shift the clips by in time.
      mode: Which partition to use, must be 'training', 'validation', or
        'testing'.
      sess: TensorFlow session that was active when processor was created.

    Returns:
      List of sample data for the transformed samples, and list of labels in
      one-hot form.
    """
        background_frequency = self.args['background_frequency']
        background_volume_range = self.args['background_volume']
        time_shift = self.args['preprocess_config_time_shift_samples']

        # Pick one of the partitions to choose samples from.
        candidates = self.data_index[mode]
        sample_count = 1
        # Data and labels will be populated and returned.
        data = np.zeros(
            (sample_count, 1, self.args['preprocess_config_n_mfcc'], self.args['preprocess_config_spectrogram_length']))
        labels = sample_count * [0]
        desired_samples = self.args['preprocess_config_desired_samples']
        use_background = self.background_data and (mode == 'training')
        time_shift = time_shift if mode == 'training' else 0
        background_frequency = background_frequency if mode == 'training' else 0
        # Use the processing graph we created earlier to repeatedly to generate the
        # final output sample data we'll use in training.
        for i in range(offset, offset + sample_count):
            # Pick which audio sample to use.
            sample_index = i
            sample = candidates[sample_index]
            # If we're time shifting, set up the offset for this sample.
            if time_shift > 0:
                time_shift_amount = np.random.randint(-time_shift, time_shift)
            else:
                time_shift_amount = 0
            if time_shift_amount > 0:
                time_shift_padding = [[time_shift_amount, 0], [0, 0]]
                time_shift_offset = [0, 0]
            else:
                time_shift_padding = [[0, -time_shift_amount], [0, 0]]
                time_shift_offset = [-time_shift_amount, 0]

            wav_filename = sample[0]

            # Choose a section of background noise to mix in.
            if use_background and self.perturb is None:
                background_index = np.random.randint(len(self.background_data))
                background_samples = self.background_data[background_index]
                background_offset = np.random.randint(
                    0, len(background_samples) - self.args['preprocess_config_desired_samples'])
                background_clipped = background_samples[background_offset:(
                        background_offset + desired_samples)]
                background_reshaped = background_clipped.reshape([desired_samples, 1])
                if np.random.uniform(0, 1) < background_frequency:
                    background_volume = np.random.uniform(0, background_volume_range)
                else:
                    background_volume = 0
            else:
                background_reshaped = np.zeros([desired_samples, 1])
                background_volume = 0
            background_data = background_reshaped
            background_volume = background_volume
            # If we want silence, mute out the main sample but leave the background.
            if sample[1] == self.args['silence_label']:
                foreground_volume = 0
            else:
                foreground_volume = 1
            # Run the graph to produce the output audio.
            data[i - offset, 0, :] = self.prepare_processing_graph(wav_filename, foreground_volume,
                                                                background_data,
                                                                background_volume, time_shift_padding,
                                                                time_shift_offset, mode)
            labels[i - offset] = self.word_to_index[sample[1]]
        return data, labels
