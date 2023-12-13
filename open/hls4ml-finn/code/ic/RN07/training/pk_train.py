import os
if os.system('nvidia-smi') == 0:
    import setGPU
import tensorflow as tf
import glob
import sys
import argparse
from tensorflow.keras.preprocessing.image import ImageDataGenerator
from sklearn.metrics import roc_auc_score
import resnet_v1_eembc
import yaml
import csv
# from keras_flops import get_flops # (different flop calculation)
import kerop
import training_pokemon as pk  #gets already formated pokemon dataset (images --> numpy arrays)
from tensorflow.keras.layers.experimental.preprocessing import RandomCrop
random_crop_model = tf.keras.models.Sequential()
random_crop_model.add(RandomCrop(32, 32, input_shape=(32, 32, 3,)))

import numpy as np
import matplotlib.pyplot as plt
from itertools import cycle

from sklearn import svm, datasets
from sklearn.metrics import roc_curve, auc
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import label_binarize
from sklearn.multiclass import OneVsRestClassifier
from sklearn.metrics import roc_auc_score
import sklearn.metrics as metrics


def random_crop(x):
    return random_crop_model.predict(x)


def get_lr_schedule_func(initial_lr, lr_decay):

    def lr_schedule_func(epoch):
        return initial_lr * (lr_decay ** epoch)

    return lr_schedule_func


def main(args):

    # parameters
    input_shape = [32, 32, 3]
    num_classes = 10
    with open(args.config) as stream:
        config = yaml.safe_load(stream)
    num_filters = config['model']['filters']
    kernel_sizes = config['model']['kernels']
    strides = config['model']['strides']
    l1p = float(config['model']['l1'])
    l2p = float(config['model']['l2'])
    skip = bool(config['model']['skip'])
    avg_pooling = bool(config['model']['avg_pooling'])
    batch_size = config['fit']['batch_size']
    num_epochs = config['fit']['epochs']
    verbose = config['fit']['verbose']
    patience = config['fit']['patience']
    save_dir = config['save_dir']
    model_name = config['model']['name']
    loss = config['fit']['compile']['loss']
    model_file_path = os.path.join(save_dir, 'model_best.h5')

    # quantization parameters
    if 'quantized' in model_name:
        logit_total_bits = config["quantization"]["logit_total_bits"]
        logit_int_bits = config["quantization"]["logit_int_bits"]
        activation_total_bits = config["quantization"]["activation_total_bits"]
        activation_int_bits = config["quantization"]["activation_int_bits"]
        alpha = config["quantization"]["alpha"]
        use_stochastic_rounding = config["quantization"]["use_stochastic_rounding"]
        logit_quantizer = config["quantization"]["logit_quantizer"]
        activation_quantizer = config["quantization"]["activation_quantizer"]
        final_activation = bool(config['model']['final_activation'])

    # optimizer
    optimizer = getattr(tf.keras.optimizers, config['fit']['compile']['optimizer'])
    initial_lr = config['fit']['compile']['initial_lr']
    lr_decay = config['fit']['compile']['lr_decay']

    # load dataset
    X_train, y_train, X_val, y_val = pk.get_pokemon_dataset()
    X_train, X_val = X_train/256., X_val/256.

    y_train = tf.keras.utils.to_categorical(y_train, num_classes)
    y_val = tf.keras.utils.to_categorical(y_val, num_classes)
    if loss == 'squared_hinge':
        y_train = y_train * 2 - 1  # -1 or 1 for hinge loss
        y_val = y_val * 2 - 1

    # define data generator
    datagen = ImageDataGenerator(
        rotation_range=15,
        width_shift_range=0.1,
        height_shift_range=0.1,
        horizontal_flip=True,
        # preprocessing_function=random_crop,
        #brightness_range=(0.9, 1.2),
        #contrast_range=(0.9, 1.2)
    )

    # run preprocessing on training dataset
    datagen.fit(X_train)

    kwargs = {'input_shape': input_shape,
              'num_classes': num_classes,
              'num_filters': num_filters,
              'kernel_sizes': kernel_sizes,
              'strides': strides,
              'l1p': l1p,
              'l2p': l2p,
              'skip': skip,
              'avg_pooling': avg_pooling}

    # pass quantization params
    if 'quantized' in model_name:
        kwargs["logit_total_bits"] = logit_total_bits
        kwargs["logit_int_bits"] = logit_int_bits
        kwargs["activation_total_bits"] = activation_total_bits
        kwargs["activation_int_bits"] = activation_int_bits
        kwargs["alpha"] = None if alpha == 'None' else alpha
        kwargs["use_stochastic_rounding"] = use_stochastic_rounding
        kwargs["logit_quantizer"] = logit_quantizer
        kwargs["activation_quantizer"] = activation_quantizer
        kwargs["final_activation"] = final_activation

    # define model
    model = getattr(resnet_v1_eembc, model_name)(**kwargs)

    # print model summary
    print('#################')
    print('# MODEL SUMMARY #')
    print('#################')
    print(model.summary())
    print('#################')

    # analyze FLOPs (see https://github.com/kentaroy47/keras-Opcounter)
    layer_name, layer_flops, inshape, weights = kerop.profile(model)

    # visualize FLOPs results
    total_flop = 0
    for name, flop, shape in zip(layer_name, layer_flops, inshape):
        print("layer:", name, shape, " MFLOPs:", flop/1e6)
        total_flop += flop
    print("Total FLOPs: {} MFLOPs".format(total_flop/1e6))

    tf.keras.utils.plot_model(model,
                              to_file="model.png",
                              show_shapes=True,
                              show_dtype=False,
                              show_layer_names=False,
                              rankdir="TB",
                              expand_nested=False)

    # Alternative FLOPs calculation (see https://github.com/tokusumi/keras-flops), ~same answer
    #total_flop = get_flops(model, batch_size=1)
    #print("FLOPS: {} GLOPs".format(total_flop/1e9))

    # compile model with optimizer
    model.compile(optimizer=optimizer(learning_rate=initial_lr),
                  loss=loss,
                  metrics=['accuracy'])

    # callbacks
    from tensorflow.keras.callbacks import EarlyStopping, ModelCheckpoint, LearningRateScheduler

    lr_schedule_func = get_lr_schedule_func(initial_lr, lr_decay)

    callbacks = [ModelCheckpoint(model_file_path, monitor='val_accuracy', verbose=verbose, save_best_only=True),
                 EarlyStopping(monitor='val_accuracy', patience=patience, verbose=verbose, restore_best_weights=True),
                 LearningRateScheduler(lr_schedule_func, verbose=verbose),
                 ]

    # train
    history = model.fit(datagen.flow(X_train, y_train, batch_size=batch_size),
                        steps_per_epoch=X_train.shape[0] // batch_size,
                        epochs=num_epochs,
                        validation_data=(X_val, y_val),
                        callbacks=callbacks,
                        verbose=verbose)

    # restore "best" model
    model.load_weights(model_file_path)
    print("Trained Model:")
    print(model)

    # get predictions
    y_pred = model.predict(X_val)


    # evaluate with test dataset and share same prediction results
    evaluation = model.evaluate(X_val, y_val)

    auc = roc_auc_score(y_val, y_pred, average='weighted', multi_class='ovr')

    print('Model test accuracy = %.3f' % evaluation[1])
    print('Model test weighted average AUC = %.3f' % auc)
    
    
    #get predict probabilities
    y_pred_proba = model.predict_on_batch(X_val)
    #print(y_pred_proba)
    
    #binarize output
    y_val_bin = label_binarize(y_val, classes=[0, 1, 2, 3, 4, 5, 6, 7, 8, 9])
    #print(y_test_bin)
    n_classes = y_val_bin.shape[1] 
    
    #creating ROC curve for train/val
    fpr = dict()
    tpr = dict()
    roc_auc = dict()

    colors = ["red", "orange", "yellow", "lime", "cyan", "teal", "blue", "mediumorchid", "hotpink", "saddlebrown"]
    labels = ["Charmander", "Ditto", "Eevee", "Gengar", "Jigglypuff", "Mew", "Pikachu", "Psyduck", "Squirtle", "Weedle"]
    
    for i in range(n_classes):
      fpr[i], tpr[i], _ = roc_curve(y_val_bin[:, i], y_pred_proba[:, i])
      plt.plot(fpr[i], tpr[i], color=colors[i], lw=2, label=labels[i])
      #print('AUC for Class {}: {}'.format(i+1, auc(fpr[i], tpr[i])))
    
    plt.plot([0, 1], [0, 1], color='black', lw=2, linestyle='--')
    plt.xlim([0.0, 1.0])
    plt.ylim([0.0, 1.05])
    plt.xlabel('False Positive Rate')
    plt.ylabel('True Positive Rate')
    plt.legend()
    plt.title('Receiver Operating Characteristic Curves for Training Sets')
    plt.savefig('foo_train.png')
    
    #creating ROC curve for test
        #load test data
    X_test, y_test = pk.get_pokemon_testSet()
    X_test = X_test/256.

    y_test = tf.keras.utils.to_categorical(y_test, num_classes)
    if loss == 'squared_hinge':
        y_test = y_test * 2 - 1
    
        #evaluating the model
#    print("Evaluating the model on the test set:", model.evaluate(X_test, y_test))
    y_test_bin = label_binarize(y_test, classes=[0, 1, 2, 3, 4, 5, 6, 7, 8, 9])
    n_classes = y_test_bin.shape[1] 
    y_pred_proba_test = model.predict_on_batch(X_test)
    np.argmax(y_pred_proba_test, axis=-1)
        
    # Compute ROC curve and ROC area for each class
    fpr = dict()
    tpr = dict()
    roc_auc = dict()
    for i in range(n_classes):
        fpr[i], tpr[i], _ = roc_curve(y_test_bin[:, i], y_pred_proba_test[:, i])
        roc_auc[i] = metrics.auc(fpr[i], tpr[i])
  
    plt.figure()
    lw = 2
    plt.plot(
        fpr[2],
        tpr[2],
        color="darkorange",
        lw=lw,
        label="ROC curve (area = %0.2f)" % roc_auc[2],
    )    
          
        #plotting ROC curve for testing
    plt.xlim([0.0, 1.0])
    plt.ylim([0.0, 1.05])
    plt.xlabel('False Positive Rate')
    plt.ylabel('True Positive Rate')
    plt.legend()
    plt.title('Receiver Operating Characteristic Curves for Testing set')
    plt.savefig('foo_test.png')
    
    #save the model
    model.save(model_file_path, save_format='h5')



if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-c', '--config', type=str, default="baseline.yml", help="specify yaml config")

    args = parser.parse_args()

    main(args)
