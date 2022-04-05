"""
 @file   01_test.py
 @brief  Script for test
 @author Toshiki Nakamura, Yuki Nikaido, and Yohei Kawaguchi (Hitachi Ltd.)
 Copyright (C) 2020 Hitachi, Ltd. All right reserved.
"""

########################################################################
# import default python-library
########################################################################
# import setGPU
import os
os.environ['TF_CPP_MIN_LOG_LEVEL']='3'#removes cuda alert for connecting to gpu
import glob
import sys
########################################################################
import pdb

########################################################################
# import additional python-library
########################################################################
import numpy
# from import
from tqdm import tqdm
from sklearn import metrics
# original lib
import common as com
import keras_model

########################################################################
# main 01_test.py
########################################################################
if __name__ == "__main__":
    # check command line args
    args = com.command_line_chk()
    
    # load parameter.yaml
    param = com.yaml_load(args.config)
    param = param["train"]

    # make output result directory
    os.makedirs(param["result_directory"], exist_ok=True)

    # load base directory
    dirs = com.select_dirs(param=param)
    tb = False

    # initialize lines in csv for AUC and pAUC
    csv_lines = []

    # loop of the base directory
    for idx, target_dir in enumerate(dirs):
        print("\n===========================")
        print("[{idx}/{total}] {dirname}".format(dirname=target_dir, idx=idx+1, total=len(dirs)))
        machine_type = os.path.split(target_dir)[1]

        print("============== MODEL LOAD ==============")
        # set model path
        model_file = "{model}/model_{machine_type}.h5".format(model=param["model_directory"],
                                                                machine_type=machine_type)
        #check if testing already conducted
        model_result_file = "{result_dir}/result.csv".format(result_dir=param['result_directory'])
        if os.path.exists(model_result_file):
            if os.stat(model_result_file).st_size != 0:
                continue

        # load model file
        if not os.path.exists(model_file):
            com.logger.error("{} model not found ".format(machine_type))
            sys.exit(-1)
        model = keras_model.load_model(model_file)
        model.summary()
        # results by type
        csv_lines.append([machine_type])
        csv_lines.append(["id", "AUC", "pAUC"])
        performance = []

        machine_id_list = com.get_machine_id_list_for_test(target_dir)

        for id_str in machine_id_list:
            # load test file
            test_files, y_true = com.test_file_list_generator(target_dir, id_str)

            # setup anomaly score file path
            anomaly_score_csv = "{result}/anomaly_score_{machine_type}_{id_str}.csv".format(
                                                                                     result=param["result_directory"],
                                                                                     machine_type=machine_type,
                                                                                     id_str=id_str)
            anomaly_score_list = []

            print("\n============== BEGIN TEST FOR A MACHINE ID ==============")
            y_pred = [0. for k in test_files]
            for file_idx, file_path in tqdm(enumerate(test_files), total=len(test_files)):
                try:
                    data = com.file_to_vector_array(file_path,
                                                    n_mels=param["feature"]["n_mels"],
                                                    frames=param["feature"]["frames"],
                                                    n_fft=param["feature"]["n_fft"],
                                                    hop_length=param["feature"]["hop_length"],
                                                    power=param["feature"]["power"],
                                                    downsample=param["feature"]["downsample"],
                                                    input_dim=param["model"]["input_dim"])
                    predictions = model.predict(data)
                    if (tb):
                        path_to_wav_file, wav_filename = os.path.split(file_path)
                        input_dat = os.path.splitext(wav_filename)[0] + "_inputs.dat"
                        prediction_dat = os.path.splitext(wav_filename)[0] + "_predictions.dat"
                        path_to_dat_dir = path_to_wav_file + "/../../../../../data/" + machine_type
                        path_to_input_dat = path_to_dat_dir + "/" + input_dat
                        path_to_prediction_dat = path_to_dat_dir + "/" + prediction_dat
                        com.save_dat(data, path_to_input_dat)
                        com.save_dat(predictions, path_to_prediction_dat)
                    errors = numpy.mean(numpy.square(data - predictions), axis=1)
                    y_pred[file_idx] = numpy.mean(errors)
                    anomaly_score_list.append([os.path.basename(file_path), y_pred[file_idx]])
                except:
                    com.logger.error("file broken!!: {}".format(file_path))
            

            # save anomaly score
            com.save_csv(save_file_path=anomaly_score_csv, save_data=anomaly_score_list)
            com.logger.info("anomaly score result ->  {}".format(anomaly_score_csv))

            # append AUC and pAUC to lists
            auc = metrics.roc_auc_score(y_true, y_pred)
            p_auc = metrics.roc_auc_score(y_true, y_pred, max_fpr=param["max_fpr"])
            csv_lines.append([id_str.split("_", 1)[1], auc, p_auc])
            performance.append([auc, p_auc])
            com.logger.info("AUC : {}".format(auc))
            com.logger.info("pAUC : {}".format(p_auc))


            print("\n============ END OF TEST FOR A MACHINE ID ============")

        # calculate averages for AUCs and pAUCs
        averaged_performance = numpy.mean(numpy.array(performance, dtype=float), axis=0)
        csv_lines.append(["Average"] + list(averaged_performance))
        csv_lines.append([])

    # output results
    result_path = "{result}/{file_name}".format(result=param["result_directory"], file_name=param["result_file"])
    com.logger.info("AUC and pAUC results -> {}".format(result_path))
    com.save_csv(save_file_path=result_path, save_data=csv_lines)