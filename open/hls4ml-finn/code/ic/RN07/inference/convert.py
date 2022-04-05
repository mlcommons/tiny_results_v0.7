import os
import json
if os.system('nvidia-smi') == 0:
    import setGPU
import tensorflow as tf
from tensorflow.keras.models import Model
from sklearn.metrics import accuracy_score
import argparse
from tensorflow.keras.datasets import cifar10
from qkeras.quantizers import quantized_bits, quantized_relu
from qkeras.qlayers import QDense, QActivation
from tensorflow.keras.regularizers import l1
from tensorflow.keras.optimizers import Adam
from tensorflow.keras.layers import Activation, Input
from tensorflow.keras.models import Sequential
from sklearn.preprocessing import LabelEncoder, StandardScaler
from sklearn.model_selection import train_test_split
from sklearn.datasets import fetch_openml
from tensorflow.keras.utils import to_categorical
import numpy as np
import pandas as pd
import yaml
import hls4ml
from qkeras.utils import _add_supported_quantized_objects
# edit depending on where Vivado is installed:
# os.environ['PATH'] = '/<Xilinx installation directory>/Vivado/<version>/bin:' + os.environ['PATH']
# or source settings before running file

PERF_SAMPLE = True

def print_dict(d, indent=0):
    align = 20
    for key, value in d.items():
        print('  ' * indent + str(key), end='')
        if isinstance(value, dict):
            print()
            print_dict(value, indent+1)
        else:
            print(':' + ' ' * (20 - len(key) - 2 * indent) + str(value))


def main(args):
    # parameters
    with open(args.config) as stream:
        our_config = yaml.safe_load(stream)
    save_dir = our_config['save_dir']
    model_file_path = os.path.join(save_dir, 'model_best.h5')

    from tensorflow.keras.models import load_model
    from qkeras.utils import _add_supported_quantized_objects
    co = {}
    _add_supported_quantized_objects(co)

    model = load_model(model_file_path, custom_objects=co)
    if bool(our_config['convert']['RemoveSoftmax']):
        input_layer = model.inputs
        output_layer = None
        for layer in model.layers:
            if layer.name == 'softmax':
                output_layer = layer.input
        model = Model(inputs=input_layer, outputs=output_layer)
        model.save(model_file_path.replace('.h5', '_nosoftmax.h5'))

    apply_patches = bool(our_config['convert']['ApplyPatches'])
    if apply_patches:
        inputs = Input((32, 32, 3), name='input_3')
        x = tf.keras.layers.experimental.preprocessing.Rescaling(1/256, name='rescaling_1')(inputs)
        outputs = model(x)
        model_rescale = Model(inputs=inputs, outputs=outputs)

    model.summary()
    tf.keras.utils.plot_model(model,
                              to_file=os.path.join(save_dir, "model.png"),
                              show_shapes=True,
                              show_dtype=False,
                              show_layer_names=False,
                              rankdir="TB",
                              expand_nested=False)

    # to check on full dataset
    _, (X_test, y_test) = cifar10.load_data()
    # to check on partial dataset
    if PERF_SAMPLE:
        script_dir = os.path.dirname(os.path.realpath(__file__))
        _idxs = np.load(os.path.join(script_dir, 'perf_samples_idxs.npy'))
        X_test = X_test[_idxs]
        y_test = y_test[_idxs]
    # use first 10 samples for building with FIFO Opt
    if bool(our_config['convert']['Build']) and bool(our_config['convert']['FIFO_opt']):
        X_test = X_test[:10]
        y_test = y_test[:10]
    # or just first 2 samples for building without FIFO Opt
    elif bool(our_config['convert']['Build']):
        X_test = X_test[:2]
        y_test = y_test[:2]
    num_samples = X_test.shape[0]

    X_test = np.ascontiguousarray(X_test, dtype=np.float32)
    if not apply_patches:
        X_test = X_test/256.
    num_classes = 10
    y_test = tf.keras.utils.to_categorical(y_test, num_classes)

    if apply_patches:
        y_keras = model_rescale.predict(X_test)
    else:
        y_keras = model.predict(X_test)

    print("Keras Accuracy:  {}".format(accuracy_score(np.argmax(y_test, axis=1), np.argmax(y_keras, axis=1))))

    np.save(os.path.join(save_dir, 'y_keras.npy'), y_keras)
    np.save(os.path.join(save_dir, 'y_test.npy'), y_test)
    np.save(os.path.join(save_dir, 'X_test.npy'), X_test)

    config = hls4ml.utils.config_from_keras_model(model, granularity='name')

    print("-----------------------------------")
    print_dict(config)
    print("-----------------------------------")

    config['Model']['ReuseFactor'] = our_config['convert']['ReuseFactor']
    config['Model']['Strategy'] = our_config['convert']['Strategy']
    config['Model']['Precision'] = our_config['convert']['Precision']

    if bool(our_config['convert']['FIFO_opt']):
        config['Model']['FIFO_opt'] = 1
    if bool(our_config['convert']['EEMBC_power']):
        config['Model']['EEMBC_power'] = 1

    config['SkipOptimizers'] = ['reshape_stream']
    
    if bool(our_config['convert']['MergedRelu']):
        config['Model']['MergedRelu'] = 1
    else:
        config['SkipOptimizers'].append('relu_merge')

    for name in config['LayerName'].keys():
        config['LayerName'][name]['Trace'] = bool(our_config['convert']['Trace'])
        config['LayerName'][name]['ReuseFactor'] = our_config['convert']['ReuseFactor']
        config['LayerName'][name]['Precision'] = our_config['convert']['Precision']
    # custom configs
    for name in our_config['convert']['Override'].keys():
        if name not in config['LayerName'].keys():
            config['LayerName'][name] = {}
        config['LayerName'][name].update(our_config['convert']['Override'][name])


    backend = our_config['convert']['Backend']
    clock_period = our_config['convert']['ClockPeriod']
    io_type = our_config['convert']['IOType']
    interface = our_config['convert']['Interface']
    if backend == 'VivadoAccelerator':
        board = our_config['convert']['Board']
        driver = our_config['convert']['Driver']
        input_type=our_config['convert']['InputType']
        output_type = our_config['convert']['OutputType']
        cfg = hls4ml.converters.create_config(backend=backend, board=board, interface=interface, clock_period=clock_period,
                                              io_type=io_type, driver=driver, input_type=input_type, output_type=output_type)
    else:
        part = our_config['convert']['XilinxPart']
        cfg = hls4ml.converters.create_config(backend=backend, part=part, clock_period=clock_period,
                                              io_type=io_type)
    cfg['HLSConfig'] = config
    cfg['InputData'] = os.path.join(save_dir, 'X_test.npy')
    cfg['OutputPredictions'] = os.path.join(save_dir, 'y_test.npy')
    cfg['KerasModel'] = model
    cfg['OutputDir'] = our_config['convert']['OutputDir']
    cfg['ApplyPatches'] = our_config['convert']['ApplyPatches']

    hls_model = hls4ml.converters.keras_to_hls(cfg)

    if our_config['convert']['FIFO_opt_json'] != "None":
        with open(our_config['convert']['FIFO_opt_json'], 'r') as f:
            maxs = json.load(f)

        new_config = cfg.copy()['HLSConfig']
        for k, v in hls_model.output_vars.items():
            filtered_max = [x['max'] for x in maxs if v.cppname in x['name']]
            if len(filtered_max) == 0:
                continue
            if len(filtered_max) > 1:
                print('WARNING! Check names of FIFOs')
            if k not in new_config['LayerName']:
                new_config['LayerName'][k] = {'StreamDepth': filtered_max[0] + 1}
            else:
                new_config['LayerName'][k]['StreamDepth'] = filtered_max[0] + 1
        for x in maxs:
            if 'in_local' in x['name']:
                new_config['LayerName']['in_local'] = {'StreamDepth': x['max'] + 1}
            elif 'out_local' in x['name']:
                new_config['LayerName']['out_local'] = {'StreamDepth': x['max'] + 1}
        cfg['HLSConfig'] = new_config
        hls_model = hls4ml.converters.keras_to_hls(cfg)

    print("-----------------------------------")
    print_dict(cfg)
    print("-----------------------------------")

    # profiling / testing
    hls_model = hls4ml.converters.keras_to_hls(cfg)

    os.makedirs(cfg['OutputDir'], exist_ok=True)
    hls4ml.utils.plot_model(hls_model, show_shapes=True, show_precision=True, to_file=os.path.join(cfg['OutputDir'], 'model_hls4ml.png'))

    if bool(our_config['convert']['Trace']):
        from hls4ml.model.profiling import compare, numerical
        import matplotlib
        matplotlib.use('Agg')
        import matplotlib.pyplot as plt

        plt.figure()
        wp, wph, ap, aph = numerical(model=model, hls_model=hls_model, X=X_test)
        plt.savefig(os.path.join(cfg['OutputDir'], 'profiling_numerical.png'), dpi=300)

        #plt.figure()
        #cp = compare(keras_model=model, hls_model=hls_model, X=X_test, plot_type="dist_diff")
        #plt.savefig(os.path.join(cfg['OutputDir'], 'profiling_compare.png'), dpi=300)

        y_hls, hls4ml_trace = hls_model.trace(X_test)
        np.save(os.path.join(save_dir, 'y_hls.npy'), y_hls)
        keras_trace = hls4ml.model.profiling.get_ymodel_keras(model, X_test)

        for layer in hls4ml_trace.keys():
            plt.figure()
            klayer = layer
            if '_alpha' in layer:
                klayer = layer.replace('_alpha', '')
            plt.scatter(hls4ml_trace[layer].flatten(), keras_trace[klayer].flatten(), s=0.2)
            min_x = min(np.amin(hls4ml_trace[layer]), np.amin(keras_trace[klayer]))
            max_x = max(np.amax(hls4ml_trace[layer]), np.amax(keras_trace[klayer]))
            plt.plot([min_x, max_x], [min_x, max_x], c='gray')
            plt.xlabel('hls4ml {}'.format(layer))
            plt.ylabel('QKeras {}'.format(klayer))
            plt.savefig(os.path.join(cfg['OutputDir'], 'profiling_{}.png'.format(layer)), dpi=300)
    else:
        hls_model.compile()
        y_hls = hls_model.predict(X_test)

    print("Keras Accuracy:  {}".format(accuracy_score(np.argmax(y_test, axis=1), np.argmax(y_keras, axis=1))))
    print("hls4ml Accuracy: {}".format(accuracy_score(np.argmax(y_test, axis=1), np.argmax(y_hls, axis=1))))

    # Bitfile time
    if bool(our_config['convert']['Build']):
        if bool(our_config['convert']['FIFO_opt']):
            from hls4ml.model.profiling import optimize_fifos_depth
            hls_model = optimize_fifos_depth(hls_model)
            our_config['convert']['OutputDir'] = our_config['convert']['OutputDir'] + "_FIFO_OPT"
            hls4ml.report.read_vivado_report(our_config['convert']['OutputDir'])
        else:
            hls_model.build(reset=False, csim=True, cosim=False, validation=False, synth=True, vsynth=False, export=True)
            hls4ml.report.read_vivado_report(our_config['convert']['OutputDir'])
        if our_config['convert']['Backend'] == 'VivadoAccelerator':
            if our_config['convert']['Driver'] == 'c':
                hls4ml.writer.vivado_accelerator_writer.VivadoAcceleratorWriter.write_header_file(
                    hls_model,
                    X_test.reshape(num_samples, -1),
                    y_test,
                    y_keras,
                    y_hls,
                    num_samples,
                    os.path.join(our_config['convert']['OutputDir'], 'sdk/common/data.h'))
                
            hls4ml.templates.VivadoAcceleratorBackend.make_bitfile(hls_model)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-c', '--config', type=str, default="baseline.yml", help="specify yaml config")

    args = parser.parse_args()

    main(args)
