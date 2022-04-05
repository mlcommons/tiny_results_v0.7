import qkeras
import numpy as np

import tensorflow as tf
from tensorflow.keras.models import Model
from tensorflow.keras.layers import Input, Dense, Activation, Flatten, BatchNormalization
from tensorflow.keras.layers import Conv2D, AveragePooling2D, MaxPooling2D, Add
from tensorflow.keras.regularizers import l1_l2
from qkeras.qlayers import QDense, QActivation
from qkeras.qconvolutional import QConv2D
from qkeras.qconv2d_batchnorm import QConv2DBatchnorm
from qkeras.qpooling import QAveragePooling2D


# define model


def resnet_v1_eembc(input_shape=[32, 32, 3], num_classes=10, l1p=0, l2p=1e-4,
                    num_filters=[16, 16,  # block 1
                                 32, 32,  # block 2
                                 64, 64  # block 3
                                 ],
                    kernel_sizes=[3, 3, 3,  # block 1
                                  3, 3, 1,  # block 2
                                  3, 3, 1  # block 3
                                  ],
                    strides=['111',  # block 1
                             '212',  # block 2
                             '212',  # block 3
                             ],
                    skip=True,
                    avg_pooling=False):

    # Input layer, change kernel size to 7x7 and strides to 2 for an official resnet
    inputs = Input(shape=input_shape)
    x = Conv2D(num_filters[0],
               kernel_size=kernel_sizes[0],
               strides=int(strides[0][0]),
               padding='same',
               kernel_initializer='he_normal',
               kernel_regularizer=l1_l2(l1=l1p, l2=l2p))(inputs)
    x = BatchNormalization()(x)
    x = Activation('relu')(x)

    # First stack
    # Weight layers
    y = Conv2D(num_filters[1],
               kernel_size=kernel_sizes[1],
               strides=int(strides[0][1]),
               padding='same',
               kernel_initializer='he_normal',
               kernel_regularizer=l1_l2(l1=l1p, l2=l2p))(x)
    y = BatchNormalization()(y)
    y = Activation('relu')(y)
    y = Conv2D(num_filters[0],
               kernel_size=kernel_sizes[2],
               strides=int(strides[0][2]),
               padding='same',
               kernel_initializer='he_normal',
               kernel_regularizer=l1_l2(l1=l1p, l2=l2p))(y)
    y = BatchNormalization()(y)

    # Overall residual, connect weight layer and identity paths
    if skip:
        x = Add()([x, y])
    else:
        x = y
    x = Activation('relu')(x)

    if len(num_filters) > 2 and num_filters[2] > 0 and strides[1] != '' and kernel_sizes[3] > 0:
        # Second stack
        # Weight layers
        y = Conv2D(num_filters[2],
                   kernel_size=kernel_sizes[3],
                   strides=int(strides[1][0]),
                   padding='same',
                   kernel_initializer='he_normal',
                   kernel_regularizer=l1_l2(l1=l1p, l2=l2p))(x)
        y = BatchNormalization()(y)
        y = Activation('relu')(y)
        y = Conv2D(num_filters[3],
                   kernel_size=kernel_sizes[4],
                   strides=int(strides[1][1]),
                   padding='same',
                   kernel_initializer='he_normal',
                   kernel_regularizer=l1_l2(l1=l1p, l2=l2p))(y)
        y = BatchNormalization()(y)

        # Adjust for change in dimension due to stride in identity
        x = Conv2D(num_filters[3],
                   kernel_size=kernel_sizes[5],
                   strides=int(strides[1][2]),
                   padding='same',
                   kernel_initializer='he_normal',
                   kernel_regularizer=l1_l2(l1=l1p, l2=l2p))(x)

        # Overall residual, connect weight layer and identity paths
        if skip:
            x = Add()([x, y])
        else:
            x = y
        x = Activation('relu')(x)

    if len(num_filters) > 4 and num_filters[4] > 0 and strides[2] != '' and kernel_sizes[6] > 0:
        # Third stack
        # Weight layers
        y = Conv2D(num_filters[4],
                   kernel_size=kernel_sizes[6],
                   strides=int(strides[2][0]),
                   padding='same',
                   kernel_initializer='he_normal',
                   kernel_regularizer=l1_l2(l1=l1p, l2=l2p))(x)
        y = BatchNormalization()(y)
        y = Activation('relu')(y)
        y = Conv2D(num_filters[5],
                   kernel_size=kernel_sizes[7],
                   strides=int(strides[2][1]),
                   padding='same',
                   kernel_initializer='he_normal',
                   kernel_regularizer=l1_l2(l1=l1p, l2=l2p))(y)
        y = BatchNormalization()(y)

        # Adjust for change in dimension due to stride in identity
        x = Conv2D(num_filters[5],
                   kernel_size=kernel_sizes[8],
                   strides=int(strides[2][2]),
                   padding='same',
                   kernel_initializer='he_normal',
                   kernel_regularizer=l1_l2(l1=l1p, l2=l2p))(x)

        # Overall residual, connect weight layer and identity paths
        if skip:
            x = Add()([x, y])
        else:
            x = y
        x = Activation('relu')(x)

    if len(num_filters) > 6 and num_filters[6] > 0 and strides[3] != '' and kernel_sizes[9] > 0:
        # Fourth stack (not complete stack)
        # Weight layers
        y = Conv2D(num_filters[6],
                   kernel_size=kernel_sizes[9],
                   strides=int(strides[3][0]),
                   padding='same',
                   kernel_initializer='he_normal',
                   kernel_regularizer=l1_l2(l1=l1p, l2=l2p))(x)
        y = BatchNormalization()(y)
        x = Activation('relu')(y)

    if len(num_filters) > 7 and num_filters[7] > 0 and strides[3] != '' and kernel_sizes[10] > 0:
        y = x
        y = Conv2D(num_filters[7],
                   kernel_size=kernel_sizes[10],
                   strides=int(strides[3][1]),
                   padding='same',
                   kernel_initializer='he_normal',
                   kernel_regularizer=l1_l2(l1=l1p, l2=l2p))(y)
        y = BatchNormalization()(y)
        x = Activation('relu')(y)

        # Overall residual, connect weight layer and identity paths
        if skip:
            y = QActivation(activation=logit_quantizer)(y)
            x = Add()([x, y])
        else:
            x = y
        x = QActivation(activation=activation_quantizer)(x)

    # Final classification layer.
    pool_size = int(np.amin(x.shape[1:3]))
    if pool_size > 1 and avg_pooling:
        x = AveragePooling2D(pool_size=pool_size)(x)
    y = Flatten()(x)
    y = Dense(num_classes,
              kernel_initializer='he_normal')(y)
    outputs = Activation('softmax', name='softmax')(y)

    # Instantiate model.
    model = Model(inputs=inputs, outputs=outputs)
    return model

# quantized model


def resnet_v1_eembc_quantized(input_shape=[32, 32, 3], num_classes=10, l1p=0, l2p=1e-4,
                              num_filters=[16, 16,  # block 1
                                           32, 32,  # block 2
                                           64, 64  # block 3
                                           ],
                              kernel_sizes=[3, 3, 3,  # block 1
                                            3, 3, 1,  # block 2
                                            3, 3, 1  # block 3
                                            ],
                              strides=['111',  # block 1
                                       '212',  # block 2
                                       '212',  # block 3
                                       ],
                              logit_total_bits=7, logit_int_bits=2, activation_total_bits=7, activation_int_bits=2,
                              alpha=1, use_stochastic_rounding=False,
                              logit_quantizer='quantized_bits', activation_quantizer='quantized_relu',
                              skip=True,
                              avg_pooling=False,
                              final_activation=True):

    logit_quantizer = getattr(qkeras.quantizers, logit_quantizer)(logit_total_bits, logit_int_bits, alpha=alpha, use_stochastic_rounding=use_stochastic_rounding)
    if activation_quantizer == 'binary_tanh':
        activation_quantizer = qkeras.quantizers.binary_tanh
    else:
        activation_quantizer = getattr(qkeras.quantizers, activation_quantizer)(activation_total_bits, activation_int_bits, use_stochastic_rounding=use_stochastic_rounding)

    # Input layer, change kernel size to 7x7 and strides to 2 for an official resnet
    inputs = Input(shape=input_shape)
    x = QConv2DBatchnorm(num_filters[0],
                         kernel_size=kernel_sizes[0],
                         strides=int(strides[0][0]),
                         padding='same',
                         kernel_quantizer=logit_quantizer,
                         bias_quantizer=logit_quantizer,
                         kernel_initializer='he_normal',
                         kernel_regularizer=l1_l2(l1=l1p, l2=l2p))(inputs)
    x = QActivation(activation=activation_quantizer)(x)

    # First stack
    # Weight layers
    y = QConv2DBatchnorm(num_filters[1],
                         kernel_size=kernel_sizes[1],
                         strides=int(strides[0][1]),
                         padding='same',
                         kernel_quantizer=logit_quantizer,
                         bias_quantizer=logit_quantizer,
                         kernel_initializer='he_normal',
                         kernel_regularizer=l1_l2(l1=l1p, l2=l2p))(x)
    y = QActivation(activation=activation_quantizer)(y)
    y = QConv2DBatchnorm(num_filters[0],
                         kernel_size=kernel_sizes[2],
                         strides=int(strides[0][2]),
                         padding='same',
                         kernel_quantizer=logit_quantizer,
                         bias_quantizer=logit_quantizer,
                         kernel_initializer='he_normal',
                         kernel_regularizer=l1_l2(l1=l1p, l2=l2p))(y)

    # Overall residual, connect weight layer and identity paths
    if skip:
        y = QActivation(activation=logit_quantizer)(y)
        x = Add()([x, y])
    else:
        x = y
    x = QActivation(activation=activation_quantizer)(x)

    if len(num_filters) > 2 and num_filters[2] > 0 and strides[1] != '' and kernel_sizes[3] > 0:
        # Second stack
        # Weight layers
        y = QConv2DBatchnorm(num_filters[2],
                             kernel_size=kernel_sizes[3],
                             strides=int(strides[1][0]),
                             padding='same',
                             kernel_quantizer=logit_quantizer,
                             bias_quantizer=logit_quantizer,
                             kernel_initializer='he_normal',
                             kernel_regularizer=l1_l2(l1=l1p, l2=l2p))(x)
        y = QActivation(activation=activation_quantizer)(y)
        y = QConv2DBatchnorm(num_filters[3],
                             kernel_size=kernel_sizes[4],
                             strides=int(strides[1][1]),
                             padding='same',
                             kernel_quantizer=logit_quantizer,
                             bias_quantizer=logit_quantizer,
                             kernel_initializer='he_normal',
                             kernel_regularizer=l1_l2(l1=l1p, l2=l2p))(y)

        # Adjust for change in dimension due to stride in identity
        x = QConv2D(num_filters[3],
                    kernel_size=kernel_sizes[5],
                    strides=int(strides[1][2]),
                    padding='same',
                    kernel_quantizer=logit_quantizer,
                    bias_quantizer=logit_quantizer,
                    kernel_initializer='he_normal',
                    kernel_regularizer=l1_l2(l1=l1p, l2=l2p))(x)
        x = QActivation(activation=logit_quantizer)(x)

        # Overall residual, connect weight layer and identity paths
        if skip:
            y = QActivation(activation=logit_quantizer)(y)
            x = Add()([x, y])
        else:
            x = y
        x = QActivation(activation=activation_quantizer)(x)

    if len(num_filters) > 4 and num_filters[4] > 0 and strides[2] != '' and kernel_sizes[6] > 0:
        # Third stack
        # Weight layers
        y = QConv2DBatchnorm(num_filters[4],
                             kernel_size=kernel_sizes[6],
                             strides=int(strides[2][0]),
                             padding='same',
                             kernel_quantizer=logit_quantizer,
                             bias_quantizer=logit_quantizer,
                             kernel_initializer='he_normal',
                             kernel_regularizer=l1_l2(l1=l1p, l2=l2p))(x)
        y = QActivation(activation=activation_quantizer)(y)
        y = QConv2DBatchnorm(num_filters[5],
                             kernel_size=kernel_sizes[7],
                             strides=int(strides[2][1]),
                             padding='same',
                             kernel_quantizer=logit_quantizer,
                             bias_quantizer=logit_quantizer,
                             kernel_initializer='he_normal',
                             kernel_regularizer=l1_l2(l1=l1p, l2=l2p))(y)

        # Adjust for change in dimension due to stride in identity
        x = QConv2D(num_filters[5],
                    kernel_size=kernel_sizes[8],
                    strides=int(strides[2][2]),
                    padding='same',
                    kernel_quantizer=logit_quantizer,
                    bias_quantizer=logit_quantizer,
                    kernel_initializer='he_normal',
                    kernel_regularizer=l1_l2(l1=l1p, l2=l2p))(x)
        x = QActivation(activation=logit_quantizer)(x)

        # Overall residual, connect weight layer and identity paths
        if skip:
            y = QActivation(activation=logit_quantizer)(y)
            x = Add()([x, y])
        else:
            x = y
        x = QActivation(activation=activation_quantizer)(x)

    if len(num_filters) > 6 and num_filters[6] > 0 and strides[3] != '' and kernel_sizes[9] > 0:
        # Fourth stack (not complete stack)
        # Weight layers
        y = QConv2DBatchnorm(num_filters[6],
                             kernel_size=kernel_sizes[9],
                             strides=int(strides[3][0]),
                             padding='same',
                             kernel_quantizer=logit_quantizer,
                             bias_quantizer=logit_quantizer,
                             kernel_initializer='he_normal',
                             kernel_regularizer=l1_l2(l1=l1p, l2=l2p))(x)
        x = QActivation(activation=activation_quantizer)(y)

    if len(num_filters) > 7 and num_filters[7] > 0 and strides[3] != '' and kernel_sizes[10] > 0:
        y = x
        y = QConv2DBatchnorm(num_filters[7],
                             kernel_size=kernel_sizes[10],
                             strides=int(strides[3][1]),
                             padding='same',
                             kernel_quantizer=logit_quantizer,
                             bias_quantizer=logit_quantizer,
                             kernel_initializer='he_normal',
                             kernel_regularizer=l1_l2(l1=l1p, l2=l2p))(y)
        x = QActivation(activation=logit_quantizer)(x)

        # Overall residual, connect weight layer and identity paths
        if skip:
            y = QActivation(activation=logit_quantizer)(y)
            x = Add()([x, y])
        else:
            x = y
        x = QActivation(activation=activation_quantizer)(x)

    # Final classification layer.
    pool_size = int(np.amin(x.shape[1:3]))
    if pool_size > 1 and avg_pooling:
        x = QAveragePooling2D(pool_size=pool_size, quantizer=logit_quantizer)(x)

    y = Flatten()(x)
    # Changed output to separate QDense but did not quantize softmax as specified
    outputs = QDense(num_classes,
                     kernel_quantizer=logit_quantizer,
                     bias_quantizer=logit_quantizer,
                     kernel_initializer='he_normal')(y)
    if final_activation:
        outputs = Activation('softmax', name='softmax')(outputs)

    # Instantiate model.
    model = Model(inputs=inputs, outputs=outputs)
    return model
