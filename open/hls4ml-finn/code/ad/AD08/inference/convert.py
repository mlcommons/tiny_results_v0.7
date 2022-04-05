from tkinter import Y
import os
# os.environ['CUDA_VISIBLE_DEVICES'] = '0'
import tensorflow as tf
import yaml
import argparse
import numpy as np
from plot_roc import plot_roc

import hls4ml
from hls4ml.converters import convert_from_keras_model
import matplotlib.pyplot as plt

def is_tool(name):
    from distutils.spawn import find_executable
    return find_executable(name) is not None

def print_dict(d, indent=0):
    align = 20
    for key, value in d.items():
        print('  ' * indent + str(key), end='')
        if isinstance(value, dict):
            print()
            print_dict(value, indent+1)
        else:
            print(':' + ' ' * (20 - len(key) - 2 * indent) + str(value))

def yaml_load(config):
        with open(config, 'r') as stream:
            param = yaml.safe_load(stream)
        return param
  
def load_model(file_path):
    from qkeras.utils import _add_supported_quantized_objects
    co = {}
    _add_supported_quantized_objects(co)  
    return tf.keras.models.load_model(file_path, custom_objects=co)


def main(args):

    config = yaml_load(args.config)
    convert_config = config['convert']

    os.environ['PATH'] = convert_config['vivado_path'] + os.environ['PATH']
    print('-----------------------------------')
    if not is_tool('vivado_hls'):
        print('Xilinx Vivado HLS is NOT in the PATH')
    else:
        print('Xilinx Vivado HLS is in the PATH')
    print('-----------------------------------')

    #define configuration parameters
    BOARD_NAME = convert_config['board_name']
    FPGA_PART = convert_config['fpga_part']
    TB_DATA_DIR = convert_config['tb_data_dir']
    X_TEST_DATA_DIR = convert_config['x_npy_dir']
    Y_TEST_DATA_DIR = convert_config['y_npy_dir']
    OUTPUT_DIR = convert_config['output_dir']
    HLS_CONFIG = convert_config['hls_config']
    CLOCK_PERIOD=convert_config['clock_period']
    BACKEND=convert_config['backend']
    IO_TYPE=convert_config['io_type']
    INTERFACE=convert_config['interface']
    DRIVER=convert_config['driver']

    # test bench datasets
    X_tb = np.load(TB_DATA_DIR, allow_pickle=True)
    X_profiling =  np.load(X_TEST_DATA_DIR, allow_pickle=True)

    model = load_model(convert_config['model_file'])
    model.summary()

    hls4ml.model.optimizer.OutputRoundingSaturationMode.layers = ['Activation']
    hls4ml.model.optimizer.OutputRoundingSaturationMode.rounding_mode = 'AP_RND'
    hls4ml.model.optimizer.OutputRoundingSaturationMode.saturation_mode = 'AP_SAT'
    hls_config = yaml_load(HLS_CONFIG)
    hls_config['Model']['Strategy'] = 'Resource'
    hls_config['Model']['Precision'] = 'ap_fixed<16,8>'
    hls_config['LayerName']['input_1']['Precision'] = 'ap_fixed<8,8>'
    for layer in hls_config['LayerName'].keys():
        hls_config['LayerName'][layer]['Trace'] = True
    
    #prevent relu_merge from being applied
    hls_config['SkipOptimizers'] = ['relu_merge']
    #set EEMBC_power config if available
    if 'EEMBC_power' in convert_config:
        if bool(convert_config['EEMBC_power']):
            hls_config['Model']['EEMBC_power'] = 1
            OUTPUT_DIR = OUTPUT_DIR + '_power'
        else:
            OUTPUT_DIR = OUTPUT_DIR + '_accuracy'

    hls_model = convert_from_keras_model(model=model,
        clock_period=CLOCK_PERIOD,
        backend=BACKEND,
        board=BOARD_NAME,
        part=FPGA_PART,
        io_type=IO_TYPE,
        interface=INTERFACE,
        driver=DRIVER,
        input_data_tb=TB_DATA_DIR,
        output_data_tb=TB_DATA_DIR,
        hls_config=hls_config,
        output_dir=OUTPUT_DIR)

    #hls4ml.utils.plot_model(hls_model, show_shapes=True, show_precision=True, to_file='{}/model_hls4ml.png'.format(OUTPUT_DIR))
    hls_model.compile()

    # profiling / testing
    PROFILE_DIR = OUTPUT_DIR+'/hls4ml_profiling_plots'
    os.makedirs(PROFILE_DIR)
    hls4ml_pred, hls4ml_trace = hls_model.trace(np.ascontiguousarray(X_profiling[0][0][0]))
    # Run tracing on a portion of the test set for the Keras model (floating-point precision)
    keras_trace = hls4ml.model.profiling.get_ymodel_keras(model, X_profiling[0][0])
    for key in hls4ml_trace:
        plt.figure()
        plt.scatter(keras_trace[key][0], hls4ml_trace[key][0], color='black')
        plt.plot(np.linspace(np.min(keras_trace[key][0]),np.max(keras_trace[key][0]), 10), 
                np.linspace(np.min(keras_trace[key][0]),np.max(keras_trace[key][0]), 10), label='keras_range')
        plt.plot(np.linspace(np.min(hls4ml_trace[key][0]),np.max(hls4ml_trace[key][0]), 10), 
                np.linspace(np.min(hls4ml_trace[key][0]),np.max(hls4ml_trace[key][0]), 10), label='hls4ml_range')
        plt.title(key)
        plt.xlabel('keras output')
        plt.ylabel('hls4ml output')
        plt.legend()

        plt.savefig(f'{PROFILE_DIR}/{key}')
        print('profiled layer {}'.format(key))

    print("-----------------------------------")
    print('Plotting AUC Curves')
    print("-----------------------------------")
    os.makedirs(f'{OUTPUT_DIR}/test', exist_ok=1)
    plot_roc(model, hls_model, X_TEST_DATA_DIR, Y_TEST_DATA_DIR,data_split_factor=1, output_dir=f'{OUTPUT_DIR}/test')

     # Bitfile time
    if bool(convert_config['Build']):
        if bool(convert_config['FIFO_opt']):
            from hls4ml.model.profiling import optimize_fifos_depth
            hls_model = optimize_fifos_depth(model, output_dir=OUTPUT_DIR,
                                             clock_period=CLOCK_PERIOD,
                                             backend=BACKEND,
                                             input_data_tb=X_tb,
                                             output_data_tb=X_tb,
                                             board=BOARD_NAME, 
                                             hls_config=hls_config)
        else:
            hls_model.build(reset=False, csim=True, cosim=True, validation=True, synth=True, vsynth=True, export=True)
            hls4ml.report.read_vivado_report(OUTPUT_DIR)
        if BACKEND == 'VivadoAccelerator':
            output_keras = model.predict(np.ascontiguousarray(X_tb))
            output_hls = hls_model.predict(np.ascontiguousarray(X_tb))
            hls4ml.writer.vivado_accelerator_writer.VivadoAcceleratorWriter.write_header_file(hls_model, X_tb, X_tb, output_keras, output_hls, 10, OUTPUT_DIR + '/sdk/common/data.h')
            hls4ml.templates.VivadoAcceleratorBackend.make_bitfile(hls_model)
    
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-c', '--config', type=str, default='ad08_pynq.yml', help="specify yaml config")

    args = parser.parse_args()

    main(args)