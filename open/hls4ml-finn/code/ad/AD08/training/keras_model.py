"""
 @file   keras_model.py
 @brief  Script for keras model definition
 @author Toshiki Nakamura, Yuki Nikaido, and Yohei Kawaguchi (Hitachi Ltd.)
 Copyright (C) 2020 Hitachi, Ltd. All right reserved.
"""

########################################################################
# import python-library
########################################################################
# from import

import tensorflow.keras as keras
import tensorflow.keras.models
from tensorflow.keras.models import Model
from tensorflow.keras.layers import Input, Dense, BatchNormalization, Activation
from tensorflow.keras.regularizers import l1
from qkeras.qlayers import QDense, QActivation
from qkeras import QDenseBatchnorm
from qkeras.quantizers import quantized_bits, quantized_relu

import logging


def get_model(name,inputDim,**kwargs):
    if name=='keras_model':
        return get_keras_model(inputDim,**kwargs)
    elif name=='qkeras_model':
        return get_qkeras_model(inputDim,**kwargs)
    elif name=='qdense_batchnorm_model':
        return get_qdense_batchnorm_model(inputDim,**kwargs)
    else:
        print('ERROR')
        return None

########################################################################
# keras model
########################################################################
def get_keras_model(inputDim,hiddenDim=128,latentDim=8, 
                    encodeDepth=4, encodeIn=128, decodeDepth=4, 
                    decodeOut=128, batchNorm=True, l1reg=0, **kwargs):
    """
    define the keras model
    the model based on the simple dense auto encoder 
    (128*128*128*128*8*128*128*128*128)
    """
    
    # Declare encode network
    inputLayer = Input(shape=(inputDim,))
    kwargs = {'kernel_regularizer': l1(l1reg)}

    for i in range(encodeDepth):
        if i==0:
            h = Dense(encodeIn,**kwargs)(inputLayer)
        else:
            h = Dense(hiddenDim,**kwargs)(h)
        if batchNorm:
            h = BatchNormalization()(h)
        h = Activation('relu')(h)
    
    #Declare latent layer
    if decodeDepth==0:
        h = Dense(latentDim,**kwargs)(inputLayer)
    else:
        h = Dense(latentDim,**kwargs)(h)
    if batchNorm:
        h = BatchNormalization()(h)
    h = Activation('relu')(h)

    # Declare decoder network
    for i in range(decodeDepth):
        if i==decodeDepth-1:
            h = Dense(decodeOut,**kwargs)(h)
        else:
            h = Dense(hiddenDim,**kwargs)(h)
        if batchNorm:
            h = BatchNormalization()(h)
        h = Activation('relu')(h)

    h = Dense(inputDim,**kwargs)(h)

    return Model(inputs=inputLayer, outputs=h)

########################################################################
# qkeras model
########################################################################
def get_qkeras_model(inputDim,hiddenDim=128,latentDim=8, 
                    encodeDepth=4, encodeIn=128, decodeDepth=4, 
                    decodeOut=128,bits=7,intBits=0,
                     reluBits=7,reluIntBits=3,lastBits=7,
                     lastIntBits=7,l1reg=0,batchNorm=True, **kwargs):
    """
    define the keras model
    the model based on the simple dense autoencoder 
    (128*128*128*128*8*128*128*128*128)
    """
    inputLayer = Input(shape=(inputDim,))
    kwargs = {'kernel_quantizer': quantized_bits(bits,intBits,alpha=1),
              'bias_quantizer': quantized_bits(bits,intBits,alpha=1), 
              'kernel_initializer': 'lecun_uniform', 
              'kernel_regularizer': l1(l1reg)
          }
    
    # Declare encoder network
    for i in range(encodeDepth):
        if i==0:
            h = QDense(encodeIn, **kwargs)(inputLayer)
        else:
            h = QDense(hiddenDim, **kwargs)(h)
        if batchNorm:
            h = BatchNormalization()(h)
        h = QActivation(activation=quantized_relu(reluBits,reluIntBits))(h)
    
    # Declare latent space
    if encodeDepth==0:
        h = QDense(latentDim, **kwargs)(inputLayer)
    else:
        h = QDense(latentDim, **kwargs)(h)
    if batchNorm:
        h = BatchNormalization()(h)
    h = QActivation(activation=quantized_relu(reluBits,reluIntBits))(h)

    # Declare decoder network
    for i in range(decodeDepth):
        if i ==decodeDepth-1:
            h = QDense(decodeOut, **kwargs)(h)
        else:
            h = QDense(hiddenDim, **kwargs)(h)
        if batchNorm:
            h = BatchNormalization()(h)
        h = QActivation(activation=quantized_relu(reluBits,reluIntBits))(h)

    kwargslast = {'kernel_quantizer': quantized_bits(lastBits,lastIntBits,alpha=1),
              'bias_quantizer': quantized_bits(lastBits,lastIntBits,alpha=1), 
              'kernel_initializer': 'lecun_uniform', 
              'kernel_regularizer': l1(l1reg)
          }
    h = QDense(inputDim, **kwargslast)(h)

    return Model(inputs=inputLayer, outputs=h)


def get_qdense_batchnorm_model(inputDim,hiddenDim=128,latentDim=8, 
                    encodeDepth=4, encodeIn=128, decodeDepth=4, 
                    decodeOut=128,bits=7,intBits=0,
                     reluBits=7,reluIntBits=3,lastBits=7,
                     lastIntBits=7,l1reg=0,batchNorm=True, **kwargs):
    """
    define the keras model
    the model based on the simple dense autoencoder 
    (128*128*128*128*8*128*128*128*128)
    """
    inputLayer = Input(shape=(inputDim,))
    kwargs = {'kernel_quantizer': quantized_bits(bits,intBits,alpha=1),
              'bias_quantizer': quantized_bits(bits,intBits,alpha=1), 
              'kernel_initializer': 'lecun_uniform', 
              'kernel_regularizer': l1(l1reg)
          }
    
    # Declare encoder network
    for i in range(encodeDepth):
        if i==0:
            h = QDenseBatchnorm(encodeIn, **kwargs)(inputLayer)
        else:
            h = QDenseBatchnorm(hiddenDim, **kwargs)(h)
        h = QActivation(activation=quantized_relu(reluBits,reluIntBits))(h)
    
    # Declare latent space
    if encodeDepth==0:
        h = QDenseBatchnorm(latentDim, **kwargs)(inputLayer)
    else:
        h = QDenseBatchnorm(latentDim, **kwargs)(h)
    h = QActivation(activation=quantized_relu(reluBits,reluIntBits))(h)

    # Declare decoder network
    for i in range(decodeDepth):
        if i ==decodeDepth-1:
            h = QDenseBatchnorm(decodeOut, **kwargs)(h)
        else:
            h = QDenseBatchnorm(hiddenDim, **kwargs)(h)
        h = QActivation(activation=quantized_relu(reluBits,reluIntBits))(h)

    kwargslast = {'kernel_quantizer': quantized_bits(lastBits,lastIntBits,alpha=1),
              'bias_quantizer': quantized_bits(lastBits,lastIntBits,alpha=1), 
              'kernel_initializer': 'lecun_uniform', 
              'kernel_regularizer': l1(l1reg)
          }
    h = QDense(inputDim, **kwargslast)(h)

    return Model(inputs=inputLayer, outputs=h)

    
#########################################################################


from qkeras.utils import _add_supported_quantized_objects
co = {}
_add_supported_quantized_objects(co)
def load_model(file_path):
    return keras.models.load_model(file_path, custom_objects=co)