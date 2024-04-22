# Introduction

This tutorial project demonstrates the main functionality of Mixer using a simple demo application that performs communication rounds indefinitely while logging informative messages over the UART
connection of each node in the network.

Please feel free to play around; you may use main.c as an entry point.


# Setup (Prerequisites)

## Windows

### install mspgcc

You can get it from here:
https://sourceforge.net/projects/mspgcc/files/Windows/mingw32/mspgcc-20120406-p20120911.zip/download

**ATTENTION**: Do **not** use the current TI (Somnium) versions from here:
http://www.ti.com/tool/msp430-gcc-opensource
Unfortunately, TI changed the internal Application Binary Interface (ABI), which
leads to incompatibilities with some functions written in assembly code. We plan
to fix these issues in the near future.

### install standard build tools

We recommend MinGW and MSYS, you can get them from here:
http://www.mingw.org/

### provide base64

If you do not have base64 installed already, you may download it from here:
http://f2ko.de/en/b64.php

### update path settings

For convenience we provide `make.bat_template`. Rename or copy it to `make.bat` and
adapt the path settings inside to your installation.

## Linux

### install mspgcc

try `sudo apt-get install gcc-msp430`

**ATTENTION**: Do **not** use the current TI (Somnium) versions from here:
http://www.ti.com/tool/msp430-gcc-opensource
Unfortunately, TI changed the internal Application Binary Interface (ABI), which
leads to incompatibilities with some functions written in assembly code. We plan
to fix these issues in the near future.

## running on local devices

To program your devices you need the BSL programming tools with support for TelosB.
They are provided, for instance, together with Contiki (http://www.contiki-os.org/download.html) in the subfolder tools/sky.

## running on FlockLab

To run experiments on FlockLab (https://www.flocklab.ethz.ch), you need a user account.
You can register here: https://user.flocklab.ethz.ch/user/user_register.php

# Build and Run

## FlockLab

call `make FLOCKLAB`

If everything is set up correctly, this will generate the file tutorial_tmote_flocklab.xml that you can directly upload to FlockLab via "add new test." After the test has
finished, you can download the results and inspect the file serial.csv.

## Local devices

If you want to run the demo application on your local devices, you call

`make hex` (or just `make`)

This produces a hex file that you can upload to your motes.

Programming the devices typically erases also the info flash area. Therefore,
the demo application asks you to enter a node ID when it boots for the first time. The
ID is then stored in the info flash so that it can be reused later on.

When you setup local experiments, please make sure to check and understand the settings
in config.h, and to adapt them based on your needs.
