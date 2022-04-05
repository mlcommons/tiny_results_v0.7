# Using PlatformIO to build

This software is build with Arduino libraries, and using the Arduino
`.ino` source file conventions, but it uses the PlatformIO cross
platform embedded tool chain to build and upload firmware.

The Arduino IDE can not be used to build this firmware.

# Installing PlatformIO


PlatformIO will work under Windows, MAC and Linux, including a
Raspberry Pi system.

PlatformIO is a Python-based system.  Therefore, it is best practice
to install PlatformIO in a Python virtualization environment,
e.g. Anaconda or VirtualEnv.

PlatformIO installation instructions are here:
https://docs.platformio.org/en/latest/core/installation.html


Installing PlatformIO on Linux will require also installing the
PlatformIO udev rules as described here:
https://docs.platformio.org/en/latest/faq.html#platformio-udev-rules

# Compiling and Uploading The Firmware

To compile the firmware
```
$ pio run

. . .

Linking .pio/build/mkrzero/firmware.elf
Building .pio/build/mkrzero/firmware.bin
Checking size .pio/build/mkrzero/firmware.elf
Advanced Memory Usage is available via "PlatformIO Home > Project Inspect"
RAM:   [==        ]  22.1% (used 7244 bytes from 32768 bytes)
Flash: [=         ]  12.5% (used 32888 bytes from 262144 bytes)
========================= [SUCCESS] Took 37.06 seconds =========================
$ 
```

To compile, upload and restart the firmware:
```
$ pio run --target upload

. . .

Verify 32888 bytes of flash with checksum.
Verify successful
done in 0.037 seconds
CPU reset.
========================= [SUCCESS] Took 42.72 seconds =========================
$ 
```

# Connecting To The Serial Port

To simply connect to the dongle serial port, e.g. for debugging or
other exploration run: `pio device monitor`


# Wiring to NDP9120 HAT
- Remove NDP9120 daughter board from RPi base board
- Connect fly wires (check NRF_GPIO_Pins and CLK_GEN_ARGS in the `perf_dummy.ino` file):
  Arduino 1.12 = D3~  : SPI Slave Select 
  Arduino 1.13 = D5~  : Ext Clk
  Arduino 1.14 = D6~  : PORSTB

  Arduino 1.01 = D11  : MOSI
  Arduino 1.08 = D12  : MISO
  Arduino 0.13 = D13~ : SCK

NB: NDP Interrupt is currently unused in this demo
