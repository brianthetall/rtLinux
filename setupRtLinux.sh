#!/bin/bash

#First Half:
sudo apt install -y bc bison flex libssl-dev make libncurses5-dev

git clone -b rpi-6.12.y --depth=1 https://github.com/raspberrypi/linux
#wget https://cdn.kernel.org/pub/linux/kernel/projects/rt/6.12/patch-6.12.43-rt12.patch.gz
#gunzip patch-6.12.43-rt12.patch.gz

cd linux
#patch -p1 < ../patch-6.12.43-rt12.patch
echo "KERNEL=kernel_2712" > .config
make bcm2712_defconfig

make menuconfig
