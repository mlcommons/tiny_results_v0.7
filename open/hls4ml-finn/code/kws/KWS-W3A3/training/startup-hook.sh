#!/usr/bin/env bash

# Install torchaudio
# From: https://pytorch.org/get-started/previous-versions/
pip install torchaudio==0.7.2 -f https://download.pytorch.org/whl/torch_stable.html

# Install Brevitas
pip install brevitas==0.6.0

# Install finn-base, with inference cost estimation
pip install onnxoptimizer==0.2.6
pip install git+https://github.com/Xilinx/finn-base.git@feature/itu_competition_21#egg=finn-base[onnx]

# Install the tensorflow datasets lib.
# This is pinned to the newest version at the time, since the one shipped with the docker image
# doesn't contain the speech_commands dataset.
pip install tensorflow-datasets==4.4.0

# Nvidia profiling with determined-ai
pip install pynvml

# Python speech features
pip install python_speech_features

# scikit-learn
pip install scikit-learn
