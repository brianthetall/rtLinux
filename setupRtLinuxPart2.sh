#!/bin/bash

#Second Half:

cd linux

#Update the generated .config file to indicate its new name:
#CONFIG_LOCALVERSION="-v8_full_preempt"

#Build the kernel
make -j4 Image.gz modules dtbs 2>&1 | tee make.log

#Copy over the new kernel:
sudo cp /boot/firmware/kernel_2712.img /boot/firmware/kernel_2712.img-backup.img
sudo cp arch/arm64/boot/Image.gz /boot/firmware/kernel_2712.img
sudo cp arch/arm64/boot/dts/broadcom/*.dtb /boot/firmware/
sudo cp arch/arm64/boot/dts/overlays/*.dtb* /boot/firmware/overlays/
sudo cp arch/arm64/boot/dts/overlays/README /boot/firmware/overlays/

#sudo reboot
