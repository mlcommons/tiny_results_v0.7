import pk_train
import hls4ml
from tensorflow.keras.utils import to_categorical
from sklearn.datasets import fetch_openml
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import LabelEncoder, StandardScaler
import numpy as np

import plotting
import matplotlib.pyplot as plt
from sklearn.metrics import accuracy_score

import argparse
import yaml

#%matplotlib inline
seed = 0
np.random.seed(seed)
import tensorflow as tf
tf.random.set_seed(seed)
import os
#os.environ['PATH'] = '/opt/Xilinx/Vivado/2019.2/bin:' + os.environ['PATH']
#from ast import literal_eval

def hls4ml_main(args):
    
    #parameters
    with open(args.config) as stream:
        config = yaml.safe_load(stream)
    save_dir = config['save_dir']
    model_file_path = os.path.join(save_dir, 'model_best.h5')
    
    print("Model file path:", model_file_path)


    #loading trained model
    from tensorflow.keras.models import load_model
    #from tensorflow.keras.models import load_weights
    from tensorflow import keras    
#    model = keras.models.load_model(model_file_path,
#                       custom_objects='QConv2DBatchnorm')

    from qkeras.utils import _add_supported_quantized_objects
    co = {}
    _add_supported_quantized_objects(co)
    model = keras.models.load_model(model_file_path,
                       custom_objects=co)

    #making hls4ml config and model
    config = hls4ml.utils.config_from_keras_model(model, granularity='model')
    print("-----------------------------------")
    print("Configuration")
    plotting.print_dict(config)
    print("-----------------------------------")
    hls_model = hls4ml.converters.convert_from_keras_model(model,
                                                           hls_config=config,
                                                           output_dir='model_1/hls4ml_prj',
                                                           part='xcu250-figd2104-2L-e')

    #compiling / predicting
    hls_model.compile()
    X_test = np.ascontiguousarray(X_test)
    y_hls = hls_model.predict(X_test)

    print("hls4ml Accuracy: {}".format(accuracy_score(np.argmax(y_test, axis=1), np.argmax(y_hls, axis=1))))

    fig, ax = plt.subplots(figsize=(9, 9))
    _ = plotting.makeRoc(y_test, y_pred, le.classes_)   #keras model
    plt.gca().set_prop_cycle(None) # reset the colors
    _ = plotting.makeRoc(y_test, y_hls, le.classes_, linestyle='--')

    from matplotlib.lines import Line2D
    lines = [Line2D([0], [0], ls='-'),
             Line2D([0], [0], ls='--')]
    from matplotlib.legend import Legend
    leg = Legend(ax, lines, labels=['keras', 'hls4ml'],
                loc='lower right', frameon=False)
    ax.add_artist(leg)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-c', '--config', type=str, default="baseline.yml", help="specify yaml config")

    args = parser.parse_args()

    hls4ml_main(args)