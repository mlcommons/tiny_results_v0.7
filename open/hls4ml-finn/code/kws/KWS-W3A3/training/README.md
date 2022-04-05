# KWS training and BIN file generation for the FINN-hls4ml MLPerf tiny submission

## Included pre-trained model and quantized bin files
* A model which was trained using the instructions below can be found here: ``training_checkpoint/QONNX_model.onnx``.
  * The model is saved in the QONNX format, which is a dialect to standard ONNX with additional quanitzaiton operators. A more detailed introduction to QONNX can be found on the [FINN blog](https://xilinx.github.io/finn//2021/11/03/qonnx-and-finn.html).
  * The model is then used by the script ``make_bin_files.py`` to generate bin files with the correct quantization.
  * Note: Exporting a new model will overwrite this pre-trained model.
* The folder ``kws_bin_files/`` contains bin files, which were quantized using the pre-trained model.
  * Note: Same as with the model, exporting new bin files will overwrite the old ones.

## Prerequisites

### Installing and configuring Determined.ai 
1. Install Determined.ai 0.16.3
   * Make sure the installation requisites are fulfilled: https://docs.determined.ai/latest/sysadmin-deploy-on-prem/requirements.html
   * Create a new virtual environment, like such: ``python3 -m venv ~/.virtualenvs/det_finn``
   * Activate the virtual environment: ``source ~/.virtualenvs/det_finn/bin/activate``
   * Install determined.ai using the ``requirements_training.txt`` to install the correct determined.ai version with pip, like such: ``pip install -r requirements_training.txt``
2. Depending on the deployment scenario changes to the template file ``kws_tf_template_finn.yaml`` may be required. In particular the following settings may require changes:
   1. The ``host_path`` variable in the section ``checkpoint_storage`` should be set to a location reachable by the determined.ai host, here the resulting checkpoints will be saved during training.
   2. The ``host_path`` variable in the section ``bind_mounts`` might need configuration and should point to the location of the Google speech commands V2 dataset, as downloaded by TensorFlow. 
3. The next steps must be executed in the determined.ai virtual environment:
4. Register the kws template with: ``det tpl set kws_tf kws_tf_template_finn.yaml``
5. Make sure the new template is visible when running: ``det tpl list``

### Installing TensorFlow and finn-base 
These two dependencies are required for generating the BIN files.
We recommend installing these into a separate virtual environment, since some dependencies between determined.ai and tensorflow conflict.
1. Create a new virtual environment, like such: ``python3 -m venv ~/.virtualenvs/tf-2_finn``
2. Activate the virtual environment: ``source ~/.virtualenvs/tf-2_finn/bin/activate``
3. Install the dependencies from the ``requirements_bin_export.txt``, like such: ``pip install -r requirements_bin_export.txt``
4. Make sure ffmpeg is installed, since it is required by ``pydub``.

## Running the training and exporting the resulting network
1. Activate the determined.ai virtual environment: ``source ~/.virtualenvs/det_finn/bin/activate``
1. Start the determined experiment with: `` bash run_training.sh ``
   * This will create a file called ``experiment_number.txt``, which will contain the experiment ID of the training job started with determined.ai.
1. Then export the QONNX model with: ``python export_training_checkpoint.py``
   * This will create a new folder called ``training_checkpoint``, which contains the exported QONNX model here: ``training_checkpoint/QONNX_model.onnx``.

## Generating BIN files
1. Activate the TensorFlow virtual environment: ``source ~/.virtualenvs/tf-2_finn/bin/activate``
2. Then run the following command to generate the bin files and validate accuracy:
```
python make_bin_files.py --bin_file_path=kws_bin_files --feature_type=mfcc
```

## Note
The folder ``tiny_results_v0.5`` contains a slimmed down version of the 
[v0.5 result repository at commit f3577af43d4b980ce358b405d27d6c79747f2e65](https://github.com/mlcommons/tiny_results_v0.5/tree/f3577af43d4b980ce358b405d27d6c79747f2e65).
In this folder only the KWS reference implementation is contained, of which parts are used to cleanly implement the BIN file export for the benchmark.
