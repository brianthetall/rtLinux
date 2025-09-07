#!/bin/bash

cd $1 #linux

#Install Kernel Modules:
doas make modules_install
#doas make install #For non-ARM?

#Copy over the new kernel:
doas cp /boot/firmware/kernel_2712.img /boot/firmware/kernel_2712.img-backup.img
doas cp arch/arm64/boot/Image.gz /boot/firmware/kernel_2712.img
doas cp arch/arm64/boot/dts/broadcom/*.dtb /boot/firmware/
doas cp arch/arm64/boot/dts/overlays/*.dtb* /boot/firmware/overlays/
doas cp arch/arm64/boot/dts/overlays/README /boot/firmware/overlays/

#doas reboot
