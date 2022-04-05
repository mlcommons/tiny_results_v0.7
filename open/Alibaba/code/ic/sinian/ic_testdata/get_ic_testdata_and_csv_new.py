import numpy as np
import pickle
import os
import struct


def to_categorical(y, num_classes=None, dtype='float32'):
    """
    tensorflow.keras.utils.to_categorical(...)

    Converts a class vector (integers) to binary class matrix.
    """
    y = np.array(y, dtype='int')
    input_shape = y.shape
    if input_shape and input_shape[-1] == 1 and len(input_shape) > 1:
        input_shape = tuple(input_shape[:-1])
    y = y.ravel()
    if not num_classes:
        num_classes = np.max(y) + 1
    n = y.shape[0]
    categorical = np.zeros((n, num_classes), dtype=dtype)
    categorical[np.arange(n), y] = 1
    output_shape = input_shape + (num_classes,)
    categorical = np.reshape(categorical, output_shape)
    return categorical

def unpickle(file):
    """load the cifar-10 data"""
    with open(file, 'rb') as fo:
        data = pickle.load(fo, encoding='bytes')
    return data

def load_cifar_10_data(data_dir, negatives=False):
    """
    Return train_data, train_filenames, train_labels, test_data, test_filenames, test_labels
    """
    meta_data_dict = unpickle(data_dir + "/batches.meta")
    cifar_label_names = meta_data_dict[b'label_names']
    cifar_label_names = np.array(cifar_label_names)

    # training data
    cifar_train_data = None
    cifar_train_filenames = []
    cifar_train_labels = []

    for i in range(1, 6):
        cifar_train_data_dict = unpickle(data_dir + "/data_batch_{}".format(i))
        if i == 1:
            cifar_train_data = cifar_train_data_dict[b'data']
        else:
            cifar_train_data = np.vstack((cifar_train_data, cifar_train_data_dict[b'data']))
        cifar_train_filenames += cifar_train_data_dict[b'filenames']
        cifar_train_labels += cifar_train_data_dict[b'labels']

    cifar_train_data = cifar_train_data.reshape((len(cifar_train_data), 3, 32, 32))
    if negatives:
        cifar_train_data = cifar_train_data.transpose(0, 2, 3, 1).astype(np.float32)
    else:
        cifar_train_data = np.rollaxis(cifar_train_data, 1, 4)
    cifar_train_filenames = np.array(cifar_train_filenames)
    cifar_train_labels = np.array(cifar_train_labels)

    cifar_test_data_dict = unpickle(data_dir + "/test_batch")
    cifar_test_data = cifar_test_data_dict[b'data']
    cifar_test_filenames = cifar_test_data_dict[b'filenames']
    cifar_test_labels = cifar_test_data_dict[b'labels']

    cifar_test_data = cifar_test_data.reshape((len(cifar_test_data), 3, 32, 32))
    if negatives:
        cifar_test_data = cifar_test_data.transpose(0, 2, 3, 1).astype(np.float32)
    else:
        cifar_test_data = np.rollaxis(cifar_test_data, 1, 4)
    cifar_test_filenames = np.array(cifar_test_filenames)
    cifar_test_labels = np.array(cifar_test_labels)

    return cifar_train_data, cifar_train_filenames, to_categorical(cifar_train_labels), \
        cifar_test_data, cifar_test_filenames, to_categorical(cifar_test_labels), cifar_label_names


if __name__ == '__main__':
    # the cifar10 dir
    cifar_10_dir = '/home/lul/dataset/cifar10/cifar-10-batches-py'
    # the seleceted 200 test samlpes from https://github.com/mlcommons/tiny/tree/master/benchmark/training/image_classification
    perf_samples_npy = 'perf_samples_idxs.npy'
    # test data bin dir, for EEMBC runner
    perf_samples_dir = 'ic01'


    if not os.path.exists(perf_samples_dir):
        os.makedirs(perf_samples_dir)

    label_output_file = open("y_labels.csv", 'w')

    _idxs = np.load(perf_samples_npy)
    train_data, train_filenames, train_labels, test_data, test_filenames, test_labels, label_names = \
        load_cifar_10_data(cifar_10_dir)

    for i in _idxs:
        _output_str = '{name},{classes},{label}\n'.format(name=test_filenames[i].decode('UTF-8')[:-3] + 'bin', classes=10, label=np.argmax(test_labels[i]))
        label_output_file.write(_output_str)
        #### default shape is HWC, transpose to CHW ####
        sample_img = np.array(test_data[i]).transpose(2,0,1).flatten()

        f = open(perf_samples_dir + '/' + test_filenames[i].decode('UTF-8')[:-3] + 'bin', "wb")
        mydata = sample_img
        myfmt = 'B' * len(mydata)
        bin = struct.pack(myfmt, *mydata)
        f.write(bin)
        f.close()

    label_output_file.close()


