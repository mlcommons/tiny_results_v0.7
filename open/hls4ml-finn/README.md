# Tiny MLPerf™ v0.7 `hls4ml-finn` Xilinx PYNQ-Z2 and Arty A7-100T Open Submission

<a href="https://fastmachinelearning.org/"><img src="https://fastmachinelearning.org/hls4ml/_images/hls4ml_logo.png" alt="hls4ml logo" width="300"/></a>
<a href="https://xilinx.github.io/finn/"><img src="https://raw.githubusercontent.com/Xilinx/finn/github-pages/docs/img/finn-logo.png" alt="FINN logo" width="300"/></a>

By the collaboration between the `hls4ml` and `FINN` teams.

* Contacts:
  * Ben Hawks, email: <bhawks@fnal.gov>, GitHub: [@ben-hawks](https://github.com/ben-hawks)
  * Nhan Tran, email: <ntran@fnal.gov>, GitHub: [@nhanvtran](https://github.com/nhanvtran)
  * Javier Duarte, email: <jduarte@ucsd.edu>, GitHub: [@jmduarte](https://github.com/jmduarte)
  * Giuseppe DiGuglielmo, email: <giuseppe.diguglielmo@columbia.edu>, GitHub: [@GiuseppeDiGuglielmo](https://github.com/GiuseppeDiGuglielmo)
  * Yaman Umuroglu, email: <yamanu@xilinx.com>, GitHub: [@maltanar](https://github.com/maltanar)
* Team members:
  * Nicolò Ghielmetti, CERN
  * Jules Muhizi, Fermilab/Harvard
  * Ryan Kastner, Jason Liang, Andy Meza, Tai Nguyen, Rushil Roy, Olivia Weng, UC San Diego
  * Hendrik Borras, Ruprecht-Karls-Universität Heidelberg
  * Scott Hauck, Shih-Chieh Hsu, Aidan Yokuda, University of Washington

## Hardware
* The first board is a TUL PYNQ-Z2 based on Xilinx Zynq SoC (See https://www.tul.com.tw/productspynq-z2.html for more information).

<img src="https://user-images.githubusercontent.com/4932543/120665525-b47d6580-c440-11eb-9e74-fb3d86673683.jpg" alt="PYNQ-Z2" width="400"/>

* The second board is a Digilent Arty A7-100T based on Xilinx Artix-7 SoC (See https://digilent.com/shop/arty-a7-artix-7-fpga-development-board/ for more information).

<img src="https://digilent.com/reference/_media/reference/programmable-logic/arty/arty-0.png" alt="Arty A7-100T" width="400"/>


## Code structure
The code/results are structured as follows:
```
hls4ml-finn
├── code
│   ├── ad
│   │   └── AD08
│   │       ├── inference
│   │       │   ├── arty-a7-100t
│   │       │   │   ├── vivado_project_accuracy
│   │       │   │   │   ├── hdf
│   │       │   │   │   └── sdk
│   │       │   │   │       └── common
│   │       │   │   │           └── harness
│   │       │   │   │               └── api
│   │       │   │   └── vivado_project_power
│   │       │   │       ├── hdf
│   │       │   │       └── sdk
│   │       │   │           └── common
│   │       │   │               └── harness
│   │       │   │                   └── api
│   │       │   └── pynq-z2
│   │       │       └── vivado_project
│   │       │           ├── hdf
│   │       │           └── sdk
│   │       │               └── common
│   │       └── training
│   │           └── trained_model
│   ├── ic
│   │   ├── CNV-W1A1
│   │   │   ├── inference
│   │   │   │   ├── arty-a7-100t
│   │   │   │   │   ├── folding_config
│   │   │   │   │   ├── models
│   │   │   │   │   └── vivado_project
│   │   │   │   │       ├── sdk
│   │   │   │   │       │   ├── common
│   │   │   │   │       │   │   └── harness
│   │   │   │   │       │   │       └── api
│   │   │   │   │       │   └── hdf
│   │   │   │   │       └── sys
│   │   │   │   │           ├── tcl
│   │   │   │   │           ├── verilog
│   │   │   │   │           └── xdc
│   │   │   │   └── pynq-z2
│   │   │   │       ├── folding_config
│   │   │   │       ├── models
│   │   │   │       └── vivado_project
│   │   │   │           ├── dataset
│   │   │   │           ├── sdk
│   │   │   │           │   ├── common
│   │   │   │           │   │   └── harness
│   │   │   │           │   │       ├── monitor
│   │   │   │           │   │       │   └── th_api
│   │   │   │           │   │       └── profile
│   │   │   │           │   │           └── th_api
│   │   │   │           │   └── hdf
│   │   │   │           └── sys
│   │   │   │               ├── docs
│   │   │   │               └── tcl
│   │   │   └── training
│   │   │       └── experiments
│   │   └── RN07
│   │       ├── inference
│   │       │   ├── arty-a7-100t
│   │       │   │   ├── vivado_project_accuracy
│   │       │   │   │   ├── hdf
│   │       │   │   │   └── sdk
│   │       │   │   │       └── common
│   │       │   │   │           └── harness
│   │       │   │   │               └── api
│   │       │   │   └── vivado_project_power
│   │       │   │       ├── hdf
│   │       │   │       └── sdk
│   │       │   │           └── common
│   │       │   │               └── harness
│   │       │   │                   └── api
│   │       │   └── pynq-z2
│   │       │       └── vivado_project
│   │       │           ├── hdf
│   │       │           └── sdk
│   │       │               └── common
│   │       └── training
│   │           └── trained_model
│   └── kws
│       └── KWS-W3A3
│           ├── inference
│           │   ├── arty-a7-100t
│           │   │   └── vivado_project
│           │   │       ├── sdk
│           │   │       │   ├── common
│           │   │       │   │   └── harness
│           │   │       │   │       └── api
│           │   │       │   └── hdf
│           │   │       └── sys
│           │   │           ├── tcl
│           │   │           ├── verilog
│           │   │           └── xdc
│           │   └── pynq-z2
│           │       └── vivado_project
│           │           ├── sdk
│           │           │   ├── common
│           │           │   │   └── harness
│           │           │   │       ├── monitor
│           │           │   │       │   └── th_api
│           │           │   │       └── profile
│           │           │   │           └── th_api
│           │           │   └── hdf
│           │           └── sys
│           │               └── tcl
│           └── training
│               ├── data
│               ├── kws_bin_files
│               ├── model
│               ├── tiny_results_v0.5
│               │   └── closed
│               │       └── reference
│               │           └── code
│               │               └── training
│               │                   └── keyword_spotting
│               │                       └── trained_models
│               │                           └── kws_ref_model
│               │                               └── variables
│               └── training_checkpoint
├── results
│   ├── arty-a7-100t
│   │   ├── ad
│   │   │   └── AD08
│   │   │       ├── accuracy
│   │   │       ├── performance
│   │   │       └── power
│   │   ├── ic
│   │   │   ├── CNV-W1A1
│   │   │   │   ├── accuracy
│   │   │   │   ├── performance
│   │   │   │   └── power
│   │   │   └── RN07
│   │   │       ├── accuracy
│   │   │       ├── performance
│   │   │       └── power
│   │   └── kws
│   │       └── KWS-W3A3
│   │           ├── accuracy
│   │           ├── performance
│   │           └── power
│   └── pynq-z2
│       ├── ad
│       │   └── AD08
│       │       ├── accuracy
│       │       ├── performance
│       │       └── power
│       ├── ic
│       │   ├── CNV-W1A1
│       │   │   ├── accuracy
│       │   │   ├── performance
│       │   │   └── power
│       │   └── RN07
│       │       ├── accuracy
│       │       ├── performance
│       │       └── power
│       └── kws
│           └── KWS-W3A3
│               ├── accuracy
│               ├── performance
│               └── power
└── systems
```
* For both the different tasks/models, there are `training` and `inference` subdirectories.
* Under `training`, there are scripts/directions to train the models as well as a pretrained model.
* Under `inference`, the Xilinx HLS, Vivado, and SDK projects will be automatically created by following the corresponding READMEs.
