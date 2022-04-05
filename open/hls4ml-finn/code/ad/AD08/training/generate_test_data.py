import common as com
import os
import numpy as np
import argparse
from tqdm import tqdm

def main(args):
    # load parameter.yaml
    param = com.yaml_load(args.config)
    convert = param["convert"]
    param = param["train"]

    # make output result directory
    os.makedirs(param["result_directory"], exist_ok=True)

    # load base directory
    target_dir = com.select_dirs(param=param)[0]
    print("target dir is {}".format(target_dir))
    machine_type = os.path.split(target_dir)[1]
    print(machine_type)
    machine_id_list = com.get_machine_id_list_for_test(target_dir,)
    print(machine_id_list)

    X = []
    y = []
    for id_str in machine_id_list:
        # load test file
        X_machine_data = []
        y_machine_data = []
        test_files, y_true = com.test_file_list_generator(target_dir, id_str)

        print("\n============== CREATING TEST DATA FOR A MACHINE ID ==============")
        for file_idx, file_path in tqdm(enumerate(test_files), total=len(test_files)):
            data = com.file_to_vector_array(file_path,
                                            n_mels=param["feature"]["n_mels"],
                                            frames=param["feature"]["frames"],
                                            n_fft=param["feature"]["n_fft"],
                                            hop_length=param["feature"]["hop_length"],
                                            power=param["feature"]["power"],
                                            downsample=param["feature"]["downsample"],
                                            input_dim=param['model']['input_dim'])
            X_machine_data.append(data)
        X.append(X_machine_data)
        y.append(y_true)
    
    #save test_data
    if not os.path.exists('test_data/anomaly_detection/'):
        os.makedirs('test_data/anomaly_detection/')
    np.save(convert['x_npy_dir'],X)
    np.save(convert['y_npy_dir'],y)
    np.save(convert['tb_data_dir'],X[0][0][0:10])

                    
                    

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-c', '--config', type=str, default="baseline.yml", help="specify yaml config")

    args = parser.parse_args()

    main(args)