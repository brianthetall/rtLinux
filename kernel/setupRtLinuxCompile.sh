#!/bin/bash

#Second Half:

cd linux

#Update the generated .config file to indicate its new name:
#CONFIG_LOCALVERSION="-v8_full_preempt"

#Build the kernel
make -j4 Image.gz modules dtbs 2>&1 | tee make.log
