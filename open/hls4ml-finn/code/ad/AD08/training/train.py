"""
 @file   00_train.py
 @brief  Script for training
 @author Toshiki Nakamura, Yuki Nikaido, and Yohei Kawaguchi (Hitachi Ltd.)
 Copyright (C) 2020 Hitachi, Ltd. All right reserved.
"""

########################################################################
# import default python-library
########################################################################

import setGPU
import os
import tensorflow as tf
import glob
import sys
import time
########################################################################
import logging
########################################################################
# import additional python-library
########################################################################
import numpy
# from import
from tqdm import tqdm
# original lib
import common as com
import keras_model
########################################################################


########################################################################
# visualizer
########################################################################
class visualizer(object):
    def __init__(self):
        import matplotlib.pyplot as plt
        self.plt = plt
        self.fig = self.plt.figure(figsize=(30, 10))
        self.plt.subplots_adjust(wspace=0.3, hspace=0.3)

    def loss_plot(self, loss, val_loss):
        """
        Plot loss curve.

        loss : list [ float ]
            training loss time series.
        val_loss : list [ float ]
            validation loss time series.

        return   : None
        """
        ax = self.fig.add_subplot(1, 1, 1)
        ax.cla()
        ax.plot(loss)
        ax.plot(val_loss)
        ax.set_title("Model loss")
        ax.set_xlabel("Epoch")
        ax.set_ylabel("Loss")
        ax.legend(["Train", "Validation"], loc="upper right")

    def save_figure(self, name):
        """
        Save figure.

        name : str
            save png file path.

        return : None
        """
        self.plt.savefig(name)


########################################################################


def list_to_vector_array(file_list,
                         msg="calc...",
                         n_mels=64,
                         frames=5,
                         n_fft=1024,
                         hop_length=512,
                         power=2.0,
                         downsample=False,
                         input_dims=640):
    """
    convert the file_list to a vector array.
    file_to_vector_array() is iterated, and the output vector array is concatenated.

    file_list : list [ str ]
        .wav filename list of dataset
    msg : str ( default = "calc..." )
        description for tqdm.
        this parameter will be input into "desc" param at tqdm.

    return : numpy.array( numpy.array( float ) )
        vector array for training (this function is not used for test.)
        * dataset.shape = (number of feature vectors, dimensions of feature vectors)
    """
    # calculate the number of dimensions
    dims = n_mels * frames
    mels = 32
    print(f'length of files_list is {len(file_list)}')

    # iterate file_to_vector_array()
    for idx in tqdm(range(len(file_list)), desc=msg):
        vector_array = com.file_to_vector_array(file_list[idx],
                                                n_mels=n_mels,
                                                frames=frames,
                                                n_fft=n_fft,
                                                hop_length=hop_length,
                                                power=power,
                                                downsample=downsample,
                                                input_dim=input_dims)
        if idx == 0:
            if downsample:
                dataset = numpy.zeros((vector_array.shape[0] * len(file_list), input_dims), float)
            else:
                dataset = numpy.zeros((vector_array.shape[0] * len(file_list), dims), float)
        dataset[vector_array.shape[0] * idx: vector_array.shape[0] * (idx + 1), :] = vector_array
    print("Shape of dataset: {}".format(dataset.shape))
    return dataset


def file_list_generator(target_dir,
                        dir_name="train",
                        ext="wav"):
    """
    target_dir : str
        base directory path of the dev_data or eval_data
    dir_name : str (default="train")
        directory name containing training data
    ext : str (default="wav")
        file extension of audio files

    return :
        train_files : list [ str ]
            file list for training
    """
    com.logger.info("target_dir : {}".format(target_dir))

    # generate training list
    training_list_path = os.path.abspath("{dir}/{dir_name}/*.{ext}".format(dir=target_dir, dir_name=dir_name, ext=ext))
    files = sorted(glob.glob(training_list_path))
    if len(files) == 0:
        com.logger.exception("no_wav_file!!")

    com.logger.info("train_file num : {num}".format(num=len(files)))
    return files
########################################################################


########################################################################
# main train.py
########################################################################
if __name__ == "__main__":                                        
                                        
    args = com.command_line_chk()

    # load parameter.yaml 
    param = com.yaml_load(args.config)
    param = param["train"]
        
    # make output directory
    os.makedirs(param["model_directory"], exist_ok=True)

    #initialize model logging
    logging.basicConfig(filename=param["model_directory"]+'/model_config.log', encoding='utf-8', level=logging.DEBUG)
    logging.debug('Begin log')

    # initialize the visualizer
    visualizer = visualizer()

    # load base_directory list
    dirs = com.select_dirs(param=param)
    print(dirs)

    # loop of the base directory
    for idx, target_dir in enumerate(dirs):
        print("\n===========================")
        print("[{idx}/{total}] {dirname}".format(dirname=target_dir, idx=idx+1, total=len(dirs)))

        # set path
        machine_type = os.path.split(target_dir)[1]
        model_file_path = "{model}/model_{machine_type}.h5".format(model=param["model_directory"],
                                                                     machine_type=machine_type)
        history_img = "{model}/history_{machine_type}.png".format(model=param["model_directory"],
                                                                  machine_type=machine_type)

        if os.path.exists(history_img):
            com.logger.info("model already exists and trained")
            continue

        
        # generate dataset
        train_data_save_load_directory = "./train_time_data/train_data_inputs_{}_frames_{}_hops_{}_fft_{}_mels_{}_power_{}.npy".format(
        param["model"]["input_dim"],param["feature"]["frames"], param["feature"]["hop_length"], 
            param["feature"]["n_fft"], param["feature"]["n_mels"], param["feature"]["power"])
        
        # if train_data available, load processed data in local directory without reprocessing wav files --saves time--
        if os.path.exists(train_data_save_load_directory):
            print("Loading train_data from {}".format(train_data_save_load_directory))
            
            train_data = numpy.load(train_data_save_load_directory)
        else:
            print("============== DATASET_GENERATOR ==============")
            files = file_list_generator(target_dir)
            train_data = list_to_vector_array(files,
                                              msg="generate train_dataset",
                                              n_mels=param["feature"]["n_mels"],
                                              frames=param["feature"]["frames"],
                                              n_fft=param["feature"]["n_fft"],
                                              hop_length=param["feature"]["hop_length"],
                                              power=param["feature"]["power"],
                                              downsample=param["feature"]["downsample"],
                                              input_dims = param["model"]["input_dim"])
            #save train_data
            if not os.path.exists('train_time_data'):
                os.makedirs('./train_time_data')
            numpy.save(train_data_save_load_directory, train_data)
            print("Train data saved to {}".format(train_data_save_load_directory))

        # train model
        print("============== MODEL TRAINING ==============")
        model = keras_model.get_model(param["model"]["name"], 
                                      inputDim = param['model']['input_dim'],
                                      hiddenDim=param["model"]["hidden_dim"], 
                                      latentDim=param["model"]["latent_dim"],
                                      encodeDepth=param["model"]["encode_depth"],
                                      encodeIn=param["model"]["encode_in"],
                                      decodeDepth=param["model"]["decode_depth"],
                                      decodeOut=param["model"]["decode_out"],
                                      batchNorm=param["model"]["batch_norm"],
                                      l1reg=param["model"]["l1reg"],
                                      bits=param["model"]["quantization"]["bits"],
                                      intBits=param["model"]["quantization"]["int_bits"],
                                      reluBits=param["model"]["quantization"]["relu_bits"],
                                      reluIntBits=param["model"]["quantization"]["relu_int_bits"],
                                      lastBits=param["model"]["quantization"]["last_bits"],
                                      lastIntBits=param["model"]["quantization"]["last_int_bits"])
        param["model"]["name"]
        model.summary()

        from tensorflow_model_optimization.python.core.sparsity.keras import prune, pruning_callbacks, pruning_schedule
        from tensorflow_model_optimization.sparsity.keras import strip_pruning, PruningSummaries
        
        begin_step = numpy.ceil(len(train_data)/param["fit"]["batch_size"]*(1.0-param["fit"]["validation_split"])).astype(numpy.int32)
        end_step = begin_step * param["fit"]["epochs"]
        
        if param["pruning"]["constant"] == True:
            pruning_params = {"pruning_schedule" : pruning_schedule.ConstantSparsity(param["pruning"]["sparsity"], begin_step=begin_step, frequency=begin_step)}
            model = prune.prune_low_magnitude(model, **pruning_params)
        elif param["pruning"]["decay"] == True:
            pruning_params = {"pruning_schedule" : pruning_schedule.PolynomialDecay(initial_sparsity=param["pruning"]["initial_sparsity"],final_sparsity=param["pruning"]["final_sparsity"],begin_step=begin_step,end_step=end_step, frequency=begin_step)}
            model = prune.prune_low_magnitude(model, **pruning_params)

        from tensorflow.keras.callbacks import EarlyStopping,History,ModelCheckpoint,ReduceLROnPlateau, TensorBoard
        
        modelbestcheck = ModelCheckpoint(model_file_path,
                                         monitor='val_loss', 
                                         verbose=1,
                                         save_best_only=True)
        stopping = EarlyStopping(monitor='val_loss',
                                 patience = 100 if param["pruning"]["constant"] == True else 100 if param["pruning"]["decay"] == True else 15, verbose=1, mode='min')
        
        reduce_lr = ReduceLROnPlateau(monitor='val_loss', factor=0.5, patience=1,
                                      mode='min', verbose=1, epsilon=0.001,
                                      cooldown=4, min_lr=1e-5)
        tensorboard = TensorBoard(log_dir = 'tensorboard_logs/{}'.format(target_dir.split('/'))[-1])
        
        callbacks=[
            modelbestcheck,
            stopping,
            reduce_lr,
        ]
        if param["pruning"]["constant"] or param["pruning"]["decay"]:
            callbacks.append(pruning_callbacks.UpdatePruningStep())
        model.compile(**param["fit"]["compile"])
        
        print("Shape of training data element is: {}".format(train_data[0].shape))
        history = model.fit(train_data,
                            train_data,
                            epochs=param["fit"]["epochs"],
                            batch_size=param["fit"]["batch_size"],
                            shuffle=param["fit"]["shuffle"],
                            validation_split=param["fit"]["validation_split"],
                            verbose=param["fit"]["verbose"],
                            callbacks=callbacks)

        if param["pruning"]["constant"] or param["pruning"]["decay"]:
            model = strip_pruning(model)
            model.save(model_file_path)
            
        visualizer.loss_plot(history.history["loss"], history.history["val_loss"])
        visualizer.save_figure(history_img)
        model.save(model_file_path)
        com.logger.info("save_model -> {}".format(model_file_path))
        print("============== END TRAINING ==============")
