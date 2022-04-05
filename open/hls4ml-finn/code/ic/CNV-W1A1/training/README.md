# CNV-W1A1 model

## Included model
The trained model file is named ``cnv-w1a1.onnx``. 
It contains a convolutional model, 
saved as an ONNX representation with some nodes in a FINN specific dialect.
The model was initially published by Umuroglu et al. in the first [FINN paper](https://arxiv.org/abs/1612.07119).

The exact model used here is nowadays part of the example networks of FINN: https://github.com/Xilinx/finn-examples/releases/download/v0.0.1a/onnx-models-bnn-pynq.zip

These were published as part of the initial release of models and bitfiles, [here](https://github.com/Xilinx/finn-examples/releases/tag/v0.0.1a).


## Model training
Even though this model is equivalent to the one from the FINN publication, 
the training of this model is now one of the examples of the quantization
aware training library [Brevitas](https://github.com/Xilinx/brevitas).

To reproduce the training the following steps should be taken:
1. Install [PyTorch](https://pytorch.org/get-started/locally/)
2. Install Brevitas, by following the install instructions here: https://github.com/Xilinx/brevitas#installation
   1. Make sure to install the export `extras` for Brevitas with: `pip install brevitas[export]`
3. Install finn-base with `pip install finn-base[onnx]`
   1. See: https://finn-base.readthedocs.io/en/latest/
4. The training can now be run as the bnn-pynq example of Brevitas
   1. The example code can be found here: https://github.com/Xilinx/brevitas/tree/master/src/brevitas_examples/bnn_pynq
   2. The command to start the training is: ``BREVITAS_JIT=1 brevitas_bnn_pynq_train --network CNV_1W1A --experiments experiments``
5. The model then needs to be exported to the FINN-ONNX format, in which the included pretrained model is saved as well.
   1. To do this run the included `export_brev_model_to_finn-onnx.py` script in the same python environment in which the training took place. This will export the most recently trained model in this folder, also adding the required pre- and post-processing for FINN.
   2. The retrained model is then exported as: `cnv-w1a1_retrained.onnx`
