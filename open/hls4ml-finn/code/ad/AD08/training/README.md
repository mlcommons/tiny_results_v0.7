# TinyML Working Group Anomaly Detection Model

# Sources
- Dataset(s)
    - https://zenodo.org/record/3678171/files/dev_data_ToyCar.zip?download=1
    - https://zenodo.org/record/3727685/files/eval_data_train_ToyCar.zip?download=1
        - *have to download both
- model topology
    - https://github.com/mlcommons/tiny/tree/master/v0.5/training/anomaly_detection

This a training environment based on the MLCommons Anomaly Detection reference model

## Quick Setup 

Run through the following commands to instantiate the training environment

```
# Download the training dataset 
./get_dataset.sh

# Download conda env if you don't already have it
wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh

bash Miniconda3-latest-Linux-x86_64.sh

# Instantiate the environment with conda using the environment file provided

conda init bash
conda env create -f environment.yml
conda activate tiny

```
## Train model
The training script `train.py` takes one argument, which is `-c` or `--config` which accepts a `.yml` model configuration file. If none is selected, a default `ad08.yml`' file in the working directory will be selected.

## Test model
After a model has been trained, the test script works similarly to the training script. Argument is `.yml` config file, default is again a  `ad08.yml` file in the working directory.