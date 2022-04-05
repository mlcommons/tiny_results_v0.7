from data.input_data_py import AudioProcessor
import torch

class PYPreprocessTransform:
    def __init__(self, hparams, profiling_prefix=None):
        self.audio_processor = AudioProcessor(hparams, profiling_prefix=profiling_prefix)
        self.hparams = hparams

    def __call__(self, index, mode):
        with torch.no_grad():
            train_fingerprints, train_ground_truth = self.audio_processor.get_data(index,  mode)
        return train_fingerprints, train_ground_truth

    def len_dataset(self, mode):
        return self.audio_processor.len_dataset(mode)
