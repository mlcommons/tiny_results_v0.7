#Import Statements
import numpy as np
import pandas as pd
import os
import random
import tensorflow as tf
from tensorflow.python.keras.models import Sequential
from tensorflow.keras.preprocessing.image import load_img, img_to_array
from tensorflow.python.keras.layers import Dense, Flatten, GlobalAveragePooling2D, Conv2D, MaxPooling2D
from tensorflow.keras.preprocessing.image import ImageDataGenerator
from tensorflow.python.keras.callbacks import ModelCheckpoint, EarlyStopping
import tensorflow_datasets as tfds

def get_pokemon_dataset():
    #getting data
#    directory = "/home/users/sayurirh/tutorial2/open/hls4ml-finn/code/ic/RN07/training/pokemondata/TEN-PokemonData"
 #   directory = "/home/users/tianafrench/hls4ml-tutorial-pynq/open/hls4ml-finn/code/ic/RN07/training/pokemon-data/TEN-PokemonData"
    directory = "/home/users/tianafrench/pokemondemo/pokemon-demo/open/hls4ml-finn/code/ic/RN07/training/TEN-PokemonData"
    dataset = tf.keras.utils.image_dataset_from_directory(
        directory,
        labels="inferred",
        label_mode="int",
        class_names=None,
        color_mode="rgb",
        batch_size=32,
        image_size=(256, 256),
        shuffle=True,
        seed=None,
        validation_split=None,
        subset=None,
        interpolation="bilinear",
        follow_links=False,
        crop_to_aspect_ratio=False,
    )
    
    #splitting data into train and validation
    image_size = (32, 32)
    batch_size = 32
    train_ds = tf.keras.preprocessing.image_dataset_from_directory(
        directory,
        validation_split=0.2,
        subset="training",
        seed=1337,
        image_size=image_size,
        batch_size=batch_size,
    )
    val_ds = tf.keras.preprocessing.image_dataset_from_directory(
        directory,
        validation_split=0.2,
        subset="validation",
        seed=1337,
        image_size=image_size,
        batch_size=batch_size,
    )
    
    #converting data format into compatible numpy arrays
        #training
    X_train = None
    y_train = None
    for image, label in tfds.as_numpy(train_ds):
        if X_train is None:
            X_train = list(image)
            y_train = list(label)
        else:
            X_train = X_train + list(image)
            y_train = y_train + list(label)
    X_train = np.array(X_train)
    y_train = np.array(y_train)
    
        #validation
    X_val = None
    y_val = None
    for image, label in tfds.as_numpy(val_ds):
        if X_val is None:
            X_val = list(image)
            y_val = list(label)
        else:
            X_val = X_val + list(image)
            y_val = y_val + list(label)
    X_val = np.array(X_val)
    y_val = np.array(y_val)
    
    return X_train, y_train, X_val, y_val

def get_pokemon_testSet():
    #getting data
#    directory = "/home/users/sayurirh/tutorial2/open/hls4ml-finn/code/ic/RN07/training/pokemondata/TEN-PokemonData"
    TEST_directory = "/home/users/tianafrench/pokemondemo/pokemon-demo/open/hls4ml-finn/code/ic/RN07/training/TEST-PokemonData"
    TEST_dataset = tf.keras.utils.image_dataset_from_directory(
        TEST_directory,
        labels="inferred",
        label_mode="int",
        class_names=None,
        color_mode="rgb",
        batch_size=32,
        image_size=(32, 32),
        shuffle=True,
        seed=None,
        validation_split=None,
        subset=None,
        interpolation="bilinear",
        follow_links=False,
        crop_to_aspect_ratio=False,
    )

    X_test = None
    y_test = None
    for image, label in tfds.as_numpy(TEST_dataset):
        if X_test is None:
            X_test = list(image)
            y_test = list(label)
        else:
            X_test = X_test + list(image)
            y_test = y_test + list(image)
    X_test = np.array(X_test)
    y_test = np.array(y_test)
    
    return X_test, y_test



