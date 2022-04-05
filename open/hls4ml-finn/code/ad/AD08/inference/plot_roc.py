from sklearn.metrics import accuracy_score
from sklearn import metrics
from sklearn.utils import shuffle
import matplotlib.pyplot as plt
# import plotting
import numpy as np
from tqdm.notebook import tqdm
import os

def plot_roc(model, hls_model, X_npy, y_npy, data_split_factor=1, output_dir=None):
    '''
    receives a keras model and an hls_model and plots 
    the roc_curve against the X_npy and y_npy. A plot
     is also created at output_dir if it is provided
    '''
    #load processed test data
    X = np.load(X_npy, allow_pickle=True)
    y = np.load(y_npy, allow_pickle=True)

    colors = ['tab:blue', 'tab:orange', 'tab:green', 'tab:purple']

    # use a quarter of the test_set to save time
    for i in range(len(X)):
        divider = int(len(X[i])/data_split_factor)
        assert len(X) == len(y)
        X[i], y[i] = shuffle(X[i], y[i])
        X[i], y[i] = X[i][0:divider],  y[i][0:divider]
    print("using 1/{} of provided dataset for roc plot".format(data_split_factor))

    fig, ax = plt.subplots(figsize=(9, 9))
    #perform inference
    for index, X_data in enumerate(X):
        keras_pred = [0. for ind in X_data]
        hls_pred = [0. for ind in X_data]
        for file_idx, X_test in enumerate(tqdm(X_data, total=len(X_data))):
            keras_predictions = model.predict(X_test)
            keras_errors = np.mean(np.square(X_test-keras_predictions), axis=1)
            keras_pred[file_idx] = np.mean(keras_errors)
            
            if hls_model is not None:
                hls_predictions = hls_model.predict(X_test)
                hls_errors = np.mean(np.square(X_test-hls_predictions), axis=1)
                hls_pred[file_idx] = np.mean(hls_errors)
        #generate auc and roc metrics
        y_test = y[index]
        k_fpr, k_tpr, k_threshold = metrics.roc_curve(y_test, keras_pred)
        k_roc_auc = metrics.auc(k_fpr, k_tpr)
        if hls_model is not None:
            h_fpr, h_tpr, h_threshold = metrics.roc_curve(y_test, hls_pred)
            h_roc_auc = metrics.auc(h_fpr, h_tpr)


        plt.title('Receiver Operating Characteristic')
        plt.plot(k_fpr, k_tpr, label = 'keras AUC m_{} = {}'.format(index, round(k_roc_auc,2)), linewidth = 1.5, color=colors[index])
        if hls_model is not None:
            plt.plot(h_fpr, h_tpr, label = 'hls AUC m_{} = {}'.format(index, round(h_roc_auc,2)), linewidth = 1, linestyle='--', color=colors[index])
        plt.legend(loc = 'lower right')
        plt.plot([0, 1], [0, 1],'r--', linewidth=1)
        plt.xlim([0, 1])
        plt.ylim([0, 1])
        plt.ylabel('True Positive Rate')
        plt.xlabel('False Positive Rate')
    plt.show()
    if output_dir != None and hls_model is not None:
        os.makedirs(output_dir, exist_ok=True)
        plt.savefig('{}/keras_vs_hls_roc_curve'.format(output_dir))
    elif output_dir != None and hls_model == None:
        os.makedirs(output_dir, exist_ok=True)
        plt.savefig('{}/keras_roc_curve'.format(output_dir))