# Introduction

This tutorial project demonstrates the main functionality of Mixer using a simple demo application that performs communication rounds indefinitely while logging informative messages over the UART
connection of each node in the network.

Please feel free to play around; you may use main.c as an entry point.


# Setup (Prerequisites)

## Get started with [Nordic nRF52840 DK](https://www.nordicsemi.com/Software-and-Tools/Development-Kits/nRF52840-DK)

Read the
[user guide](https://www.nordicsemi.com/DocLib/Content/User_Guides/nrf52840_dk/latest/UG/nrf52840_DK/intro)
([PDF version](https://www.nordicsemi.com/-/media/DocLib/Other/User_Guides/nRF52840DKUserGuidev12pdf.pdf))
and install the
[Segger Embedded Studio (SES)](https://www.nordicsemi.com/Software-and-Tools/Development-Tools/Segger-Embedded-Studio)
for ARM IDE (tested with version 5.24), which is free for use with nRF devices. It is available for Windows, Linux, and OSX. The Nordic SDK and command line tools (mentioned in the Nordic user guide) are optional for the Mixer tutorial project.

Inside SES, we suggest to install the "nRF CPU Support Package" (menu "Tools" -> "Package Manager"), which simplifies the creation of new projects targeting nRF SoCs.

## Open tutorial project

Open the tutorial project `tutorial.emProject`.

When you setup experiments, please make sure to check and understand the settings in `mixer_config.h`, and to adapt them based on your needs.


# Build and Run

Build the tutorial project in SES. The output, including .elf and .hex file, is placed in subfolder `Output/<configuration name>`.

The simplest way to program your device is to just copy the .hex file to the device via file system.
For this purpose, the nRF52840 DK gets mounted as a storage device when you plug in the USB cable.

After programming, you can connect to the virtual COM port provided by the nRF52840 DK. The serial connection parameters are 115200,8,N,1.

The tutorial project stores the individual node ID in the SoC's "User Information Configuration Registers (UICR)". Programming the device typically erases the UICR region. Therefore,
the demo application asks you to enter a node ID when it boots for the first time. The
ID is then stored in the UICR so that it can be reused later on.
