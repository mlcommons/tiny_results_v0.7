## Arty A7-100T Energy Benchmark Setup and Hookup Guide

### Components/Bill of Materials

-   [Arty A7-100T Board](https://www.newark.com/digilent/410-319-1/dev-board-fpga-artix-7/dp/40AH1638)
-   Micro USB-B cable
-   Host Computer with a USB Port (Linux/Windows/OSX, preference in that order)
-    [EEMBC EnergyRunner Software](https://github.com/eembc/energyrunner) (Binary download available through [EEMBC Website](https://www.eembc.org/) once signed up/approved)
    -   Relevant dataset `.bin` files (Anomoly Detection, Image Classification, and Keyword Spotting for the HLS4ML/Xilinx joint submission)
-   [Joulescope JS110](https://www.digikey.com/en/products/detail/joulescope%C2%AE/JS110-K000-001/13686734)
-   [Breadboard (Pololu 400pt)](https://www.digikey.com/en/products/detail/pololu-corporation/351/11586984) 
-   [Bi-Directional Level Shifters (BSS138x4 Board)](https://www.digikey.com/en/products/detail/adafruit-industries-llc/757/4990756)
-   [Arduino UNO ("IO Manager")](https://www.digikey.com/en/products/detail/arduino/A000066/2784006)
-   [Header Pins (Extra Tall)](https://www.digikey.com/en/products/detail/samtec-inc/MTSW-110-09-S-S-330/8162605) 
-   [Breadboard Jumper Wires (M-M, M-F, F-F)](https://www.amazon.com/EDGELEC-Breadboard-Optional-Assorted-Multicolored/dp/B07GD2BWPY)
-   [Stackable 4mm Banana Plug Test Leads](https://www.amazon.com/Sumnacon-Stackable-Silicone-Multimeter-Electrical/dp/B07179VF5J/)
-   [4mm Banana Plug to Jumper Wire Test Leads](https://www.amazon.com/Goupchn-Stackable-Breadboard-Flexible-Electrical/dp/B08KZGPTLM?th=1) (Male or Female, you need both but can use the Breadboard jumper wires to convert)
-   [Powered USB 3.0 Hub (with at least 3 Ports)](https://www.digikey.com/en/products/detail/adafruit-industries-llc/757/4990756) 
-   [USB-B → USB-A Cable](https://www.amazon.com/StarTech-com-3-Feet-USB-Certified-Cable/dp/B001GTW5Z2) (2 x Total, One for the Joulescope (Included), one for the Arduino UNO (Not included))
-   A 5-12V+, >3A Low Noise, Variable Power Supply with 4MM Banana Plug Outputs (This can vary, but the lowest noise/ripple you can get is desirable. Avoid cheap, no-name brands if possible. lower noise means more accurate/better results)
    -   [Siglent SPD3303C](https://www.amazon.com/Siglent-Technologies-SPD3303C-Power-Supply/dp/B01HENYNZS)
    -   [Miniware MDP-P905](https://www.amazon.com/SainSmart-MDP-P905-Programmable-Adjustable-Regulator/dp/B07ZCJGP1V) \*\*AND\*\* [iFi iPower Low Noise Travel Adapter (5V)](https://www.amazon.com/iPower-Supply-International-Travel-Adapters/dp/B01LZD8SHJ)
    -   [HP/Agilent E3610A](https://www.bellnw.com/manufacturer/Agilent/E3610A.htm) (What was used for benchmarking)

### Soldering Required

For the energy Setup, some minor soldering needs to be done. Specifically:

-   Soldering 2 1x6 extra tall header pins onto the Bidirecctional level shifter (BSS138x4) breakout board


#### Bidirectional Level Shifter Header Pins

This soldering job relativley easy, and consists of soldering the extra tall header pins onto the BSS138 Breakout board. The correct orientation is so that the gold plated ends of the header pins are sticking out of the top of the BSS138 Breakout board, with the plastic spacer underneath the board itself, so that when plugged into a breadboard it sits on the plastic spacer 

![](img/img_20210607_140451.jpg)

BSS138 Breakout Board with pins soldered inserted into breadboard

![](img/img_20210607_133515.jpg)

BSS138 Breakout Board with Extra Tall header pins in correct orientation

### Setup

To setup for an Energy Consumption Benchmark:

1.  Connect everything according to this wiring diagram where the DUT is the Arty A7-100T (All devices powered off)

![](img/hookup-js110-arty.png)

MLPerf Tiny V0.7 Energy Benchmark Wiring Diagram for Arty A7-100T

1.  Power on the Powered USB hub, this will also power the Arduino UNO ("IO Manager") & Joulescope, but no power will be coming out of the Joulescope output at this point
2.  Power on the Low Noise DC Power supply, setting it to output **7V** (Different than the Pynq!)
3.  If you haven't already, flash the Arduino UNO with the IOManager `.hex` file (available from the [EEMBC download page](https://www.eembc.org/download2/), login required) via your preferred flashing program ([AVRDUDE](https://www.nongnu.org/avrdude/), [XLoader](https://github.com/binaryupdates/xLoader), [AVRDUDESS](https://github.com/zkemble/AVRDUDESS), etc.)
4.  Connect the Arty A7-100T to the Host Computer via micro USB-B cable
5.  If you haven't already, follow the instructions in the README.md of the hls4ml-FINN Submission to build/run the model on the [v0.7 Readme](README.md)
6.  ***_Flash the board with the SREC SPI Bootloader as described in the [README](arty_bootloader.md)_***
7.  Disconnect the MicroUSB Cable from the Arty A7-100T
8.  Connect the Joulescope's positive output to the Arty A7-100T's (Arduino layout) header J7 "VIN" Terminal
9.  With the submission running on the board, open EEMBC's EnergyRunner

	9a.  If you haven't already, note the “Dataset Folder” location shown at the bottom of the “BENCHMARKS AND TEST SCRIPTS” box and move your `.bin` files to that location, in the format of a directory named the benchmark id (`kws01, ic01, ad01, vww01` etc.) containing that benchmark's `.bin` files.
10.  Follow the instructions from the [Main hls4ml-FINN v0.7 README](README.md) to run the benchmark  

Additional photos of the actual setup have been included below for reference (if there is any question between these and the diagram, follow the wiring diagram above)

![](img/full_energy_setup_arty.jpg)

MLPerf Tiny V0.7 Energy Benchmark Physical Setup mirroring the above wiring diagram

![](img/arty_tinymlpower_wiring.jpg)

MLPerf Tiny V0.7 Energy Benchmark wiring for Arty A7-100T

![](img/levelshifter_tinymlpower_wiring.png)

MLPerf Tiny V0.7 Energy Benchmark wiring for Level Shifter

![](img/arduino_tinymlpower_wiring.png)

MLPerf Tiny V0.7 Energy Benchmark wiring for Arduino UNO ("IO Manager")
