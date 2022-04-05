# load tiny MLPerf reference submission from submodule
import sys
sys.path.insert(0, "tiny_results_v0.5/closed/reference/code/training/keyword_spotting")

import os
import numpy as np
import matplotlib.pyplot as plt

import get_dataset as kws_data
import kws_util
import keras_model as models
from finn.core.modelwrapper import ModelWrapper
from qonnx.util.cleanup import cleanup_model
import finn.core.onnx_exec as oxe

if __name__ == '__main__':
    Flags, unparsed = kws_util.parse_command()

    num_test_files = Flags.num_bin_files
    test_file_path = Flags.bin_file_path

    print(f"Extracting {num_test_files} to {test_file_path}")
    word_labels = ["Down", "Go", "Left", "No", "Off", "On", "Right",
                   "Stop", "Up", "Yes", "Silence", "Unknown"]

    num_labels = len(word_labels)
    # Change into the directory of the reference implementation to execute this properly
    curr_wd = os.getcwd()
    os.chdir('tiny_results_v0.5/closed/reference/code/training/keyword_spotting')
    ds_train, ds_test, ds_val = kws_data.get_training_data(Flags, val_cal_subset=True)
    os.chdir(curr_wd)

    if Flags.target_set[0:3].lower() == 'val':
        eval_data = ds_val
        print("Evaluating on the validation set")
    elif Flags.target_set[0:4].lower() == 'test':
        eval_data = ds_test
        print("Evaluating on the test set")
    elif Flags.target_set[0:5].lower() == 'train':
        eval_data = ds_train
        print("Evaluating on the training set")

    model_settings = models.prepare_model_settings(num_labels, Flags)

    if Flags.feature_type == "mfcc":
        # output_type = np.int8
        # quant_min, quant_max = -128, 127
        # # we should really do both of these in the way that the LFBE is doing
        # # since the MFCC style depends on a specific TFL model, but since
        # # now (4/24/21) bin files for mfcc features are already published,
        # # we'll wait until v0.2 to unify the bin file quantization calibration
        # interpreter = tf.lite.Interpreter(model_path=Flags.tfl_file_name)
        # interpreter.allocate_tensors()
        # input_details = interpreter.get_input_details()
        # output_details = interpreter.get_output_details()
        # input_shape = input_details[0]['shape']
        # input_shape[0] = 0
        # input_scale, input_zero_point = input_details[0]["quantization"]

        # Open the QONNX model
        finn_model = ModelWrapper("training_checkpoint/QONNX_model.onnx")
        finn_model = cleanup_model(finn_model)
        quant_list = finn_model.get_nodes_by_op_type("Quant")
        input_quant = quant_list[0]
        input_quant_successor = finn_model.find_direct_successors(input_quant)[0]
        # check if the input is signed
        if input_quant.attribute[1].i == 1:
            output_type = np.int8
        else:
            output_type = np.uint8
        # Get scale and zero point
        input_scale = finn_model.get_initializer(input_quant.input[1])
        input_zero_point = finn_model.get_initializer(input_quant.input[2])
        # Get network input shape
        input_shape = finn_model.get_tensor_shape(finn_model.graph.input[0].name)
    else:
        raise ValueError(f"Only MFCC features are supported, but {Flags.feature_type} was requested")

    print(f"Scale factor = {input_scale}, zero point = {input_zero_point}")
    output_data = []
    labels = []
    file_names = []
    count = 0

    # set true to run the TFL model on each input before writing it to files.
    # This will also generate a file tflm_labels.csv (similar to y_labels.csv)
    # recording what the model predicted for each input
    test_tfl_on_bin_files = True

    all_dat = np.zeros(input_shape, dtype='float32')
    all_dat_q = np.zeros(input_shape, dtype=output_type)

    # make the target directory and all directories above it if it doesn't exist
    os.makedirs(test_file_path, exist_ok=True)

    eval_data = eval_data.unbatch().batch(1).take(num_test_files).as_numpy_iterator()
    for dat, label in eval_data:
        # Transpose data to match the expected shape of input_shape
        dat = dat.transpose((0, 3, 2, 1))
        # Apply scale and zero point
        dat_q = np.array(dat / input_scale + input_zero_point)
        # Clamping & rounding
        dat_q = np.where(dat_q > 127., 127., dat_q)
        dat_q = np.where(dat_q < -127., -127., dat_q)
        dat_q = np.round(dat_q)
        # Match input data type
        dat_q = dat_q.astype(output_type)

        label_str = word_labels[label[0]]
        fname = f"tst_{count:06d}_{label_str}_{label[0]}.bin"
        with open(os.path.join(test_file_path, fname), "wb") as fpo:
            fpo.write(dat_q.flatten())

        if test_tfl_on_bin_files:
            # For proper execution in the QONNX format this requires rescaling
            tmp_qdat = dat_q.astype(np.float32)
            tmp_qdat = (tmp_qdat - input_zero_point) * input_scale
            input_dict = {input_quant_successor.input[0]: tmp_qdat.astype(np.float32)}
            # This should ideally be start_node = input_quant_successor,
            # but unfortunately that's currently bugged for QONNX models in finn-base
            start_node = finn_model.graph.node[1]
            output_dict = oxe.execute_onnx(finn_model, input_dict,
                                           return_full_exec_context=True, start_node=start_node)
            current_result = output_dict[finn_model.graph.output[0].name]
            output_data.append(np.argmax(current_result))

        labels.append(label[0])
        file_names.append(fname)
        all_dat = np.concatenate((all_dat, dat))
        all_dat_q = np.concatenate((all_dat_q, dat_q))
        count += 1

    print(f"FP32      feature data ranges from {np.min(all_dat)} to {np.max(all_dat)} with mean = {np.mean(all_dat)}")
    print(
        f"Quantized feature data ranges from {np.min(all_dat_q)} to {np.max(all_dat_q)} with mean = {np.mean(all_dat_q)}")

    with open(os.path.join(test_file_path, "y_labels.csv"), "w") as fpo_true_labels:
        for (fname, lbl) in zip(file_names, labels):
            fpo_true_labels.write(f"{fname}, {num_labels}, {lbl}\n")

    if test_tfl_on_bin_files:
        num_correct = np.sum(np.array(labels) == output_data)
        acc = num_correct / len(labels)
        print(f"Accuracy = {acc:5.3f} ({num_correct}/{len(labels)})")
        with open(os.path.join(test_file_path, "tflm_labels.csv"), "w") as fpo_tflm_labels:
            for (fname, out) in zip(file_names, output_data):
                fpo_tflm_labels.write(f"{fname}, {num_labels}, {out}\n")

    fig, axes = plt.subplots(3, 1, figsize=(6, 8))
    axes[0].plot(all_dat.flatten(), all_dat_q.flatten(), 'r.')
    axes[1].hist(all_dat.flatten(), bins=20);
    axes[2].hist(all_dat_q.flatten(), bins=20);
    plt.savefig(f"test_data_quantization_{Flags.feature_type}.png")
