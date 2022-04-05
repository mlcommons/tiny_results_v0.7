## Build stitched-IP for MLPerf tiny and bitfile for Arty A7-100T

The build is currently configured for the Arty A7-100T board and a throughput of about 200k FPS at a clock frequency of 100 MHz.

1. Copy the trained `QONNX_model.onnx` model from the training folder. The command is: `cp ../../training/training_checkpoint/QONNX_model.onnx .`
2. (Optional) Download the pre-processed validation data using the `get-kws-data.sh` script.
3. Launch the build as follows:
```shell
# cd to where the FINN compiler was cloned to
cd ../finn
# launch the build on the build folder
bash run-docker.sh build_custom ../arty-a7-100t
```

4. The generated outputs will be exported to a folder called `<timestamp>_output_<onnx_file_name>_<platform>`. 
You can find a description of the generated files [here](https://finn-dev.readthedocs.io/en/latest/command_line.html#simple-dataflow-build-mode).
The folder will additionally include the quantized inputs for verification (`all_validation_KWS_data_inputs_len_10102.npy`) and the expected outputs (`all_validation_KWS_data_outputs_len_10102.npy`).
When running the network on hardware the validation should achieve an accuracy of about 84.6 %. Note that this accuracy is NOT the test accuracy, but the validation accuracy. Verifying the test accuracy can be done with the the bare metal application and the re-quantized bin files described in the next section.

## Creating the bare metal application for MLPerf tiny
The following steps are adapted from original instructions by Giuseppe:

1. Make sure the shell has access to the Vivado 2019.1 tools and SDK, by setting the PATH environment variable to include them.
2. For the Arty additionally make sure that the board definitions are installed to your Vivado installation.
   1. The board definition can be found here: https://github.com/Digilent/vivado-boards
   2. Instructions for the installation are here: https://digilent.com/reference/programmable-logic/guides/installing-vivado-and-sdk?redirect=1#installing_digilent_board_files
   3. Be sure to install the board definitions from the mater branch of the mentioned GitHub repository.
3. Two separate builds for accuracy/timing and for power measurements can be synthesised. To select which one is built line 6 in the file `vivado_project/sys/tcl/script_axidma.tcl` must be changed.
   1. Accuracy/timing measurements: Set the line to: `set eembc_power 0`
   2. Power measurements: Set the line to: `set eembc_power 1`
4. Run the Vivado project script: 

```shell
cd vivado_project/sys
make sys
```

5. The Vivado script essentially opens the FINN project and exports the HDF file for the SDK in the directory `vivado_project/sdk/hdf/`. Note that when we use HLS datamovers we don't have to create our own Vivado project (as with the AXI DMA IP).
6. Depending on which benchmarks you synthesised the design for, bootloader generation must be enabled. To enable/disable, uncomment/comment out line 10 in the file `vivado_project/sdk/script.tcl`
   1. Accuracy/timing measurements: Comment out line 10 to disable bootloader generation
   2. Power measurements: uncomment line 10 to enable bootloader generation
7. Finally, let's create the SDK project + running it:
```shell
cd vivado_project/sdk
make sdk
make gui
```
8. This will pop up the SDK, close the "welcome" tab and you should have the bare metal app.
9. The SDK project now contains all harness files to build the harness and run the accelerator + harness on the FPGA.
10. Program the FPGA with the bit file in the SDK. The screenshot below highlights the corresponding context menu and button to press.
 * <img width="600" alt="Screen Shot 2021-06-06 at 10 20 14 PM" src="https://user-images.githubusercontent.com/4932543/120962896-78ceee00-c715-11eb-8888-540dcf3bed39.png"/>
11. Build the software project, **then** run test harness software in the SDK. The screenshot below highlights the button for running the harness. The button for building the software project can be found in the same context menu, further above the "Run as" filed.
 * <img width="600" alt="Screen Shot 2021-06-06 at 10 22 07 PM" src="https://user-images.githubusercontent.com/4932543/120963020-b6337b80-c715-11eb-93c7-e0de1fa2c070.png"/>
12. Download EEMBC runner GUI and for the dataset the KWS bin files have been re-quantized for this network, they can be found in the following folder, relative to here: `../../training/kws_bin_files`


## Running benchmarks
**Before proceeding, make sure the dataset `.bin`  and `y_labels.csv` files are in the location specified by the runner!**


If you're intending to run **energy (power consumption)** benchmarks, please refer to the connection and setup guide below for your relevant board **before proceeding**:

- [Arty A7-100T Energy Benchmark Hookup and Setup Guide](arty_energy_setup.md)

### **Accuracy and/or Latency (Performance)** benchmarks:

#### Arty A7-100T
0. If you haven't already, download and open "EEMBC EnergyRunner Framework x.x.x" ("EEMBC EnergyRunner Framework 3.0.10" at time of this release)
1. Mount the Device by clicking the "Mount?" toggle in the devices list

	![](img/mount_dut1a.png)

2. Once mounted, Under "Benchmarks and Test Scripts", select "ML Performance x.x.x " ("ML Performance 1.0.1" at the time of this release)

	![](img/mount_device2.png)

3. If running **latency (performance)** benchmarks, enter the number of inferences you to run (official runs require at least 10s of runtime or 10 inferences, the number will vary depending on model/device) in the "Inferene Iterations" of the "Setup" portion. ("Warmup Iterations" can be left at 1)

	![](img/setup_test1.png)

4. Press the "Initalize" to initalize the EEMBC Runner with the appropriate benchmark info, you should see some blue highlighted output in the console and some of the runner's fields populated with info about the current model on the board

	![](img/init_runner2.png)
	![](img/init_runner1.png)

	4a. **IMPORTANT NOTE!** Due to a initalization/reset issue with the internal timer and the soft microblaze CPU core on the Arty, the test harness **_MUST_** be relaunched via the harness in order to have proper timestamps (and therefor latency results) during the test. This shouldn't impact performance, nor does it impact Energy benchmarks since those are timed via an external source. 
		![](img/init_runner1.png)
5. If running the **latency (performance)** benchmark, select the "Median Performance" radio button, otherwise if running the **accuracy**  select the "Accuracy" Radio button, then press "Run" to start the benchmark

	![](img/test_selection.png)

6. Once completed, the results of the benchmark will be printed out to the console, along with relevant logs (results included) being saved to the `~/eembc/runner/sessions` in a directory named the timestamp of when the benchmark completes

### **Energy Consumption (Power)** benchmarks:

#### Arty A7-100T
0. If you haven't already, download and open "EEMBC EnergyRunner Framework x.x.x" ("EEMBC EnergyRunner Framework 3.0.10" at time of this release)
1. Make sure you have flashed the SREC SPI Bootloader to the Arty so that it's able to cold boot the accelerator and Test Harness, as described in the [Arty Bootloader Readme](arty_bootloader.md)
2. Make sure you've connected all required devices as described in the [Arty Energy Setup Readme](arty_energy_setup.md)
1. Mount the iomanager (Arudino Uno) and energy monitor (Joulescope) in the energy runner

	![](img/run_power1.png)

2. Once mounted, Under "Benchmarks and Test Scripts", select "ML Energy x.x.x " ("ML Performance 1.0.1" at the time of this release)

	![](img/run_power2.png)
	
3. **_Make sure the power supply voltage is set to 7 Volts!_**, then press initalize to initalize the runner with test info. _**This will take up to 25 seconds!**_ During this time, the Arty will briefly power on, send a message via the iomanager, then power back off. 

	![](img/run_power_init.png)

5. If initilization is successful, you will see information about the loaded model in the device. Enter the number of inferences you to run (official runs require at least 10s of runtime or 10 inferences, the number will vary depending on model/device) in the "Inferene Iterations" of the "Setup" portion. ("Warmup Iterations" can be left at 1)

	![](img/run_power3.png)

6. Select the "Median Energy" radio button, then press "Run" to start the benchmark

	![](img/run_power4.png)

6. Once completed, the results of the benchmark will be printed out to the console, with a plot of energy usage over the duration of the benchmark displayed below the console.  along with relevant logs (results included) being saved to the `~/eembc/runner/sessions` in a directory named the timestamp of when the benchmark completes
