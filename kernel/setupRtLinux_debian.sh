#!/bin/bash

kernelSrcPath='/usr/src/linux-source-6.12'
kernelCfgPath='/usr/src/linux-config-6.12'

doas apt update && doas apt install -y linux-source

#Uncompress the kernel src:
cd /usr/src
doas unxz linux-source-*.tar.xz
doas tar -xf linux-source-*.tar

#Get the RT config file:
cd ${kernelCfgPath}
doas unxz config.amd64_rt_amd64.xz
doas cp config.amd64_rt_amd64 ${kernelSrcPath}/.config

doas ${kernelSrcPath}/scripts/config --disable DEBUG_INFO
doas ${kernelSrcPath}/scripts/config --disable DEBUG_INFO_DWARF_TOOLCHAIN_DEFAULT

cd ${kernelSrcPath}
#make localmodconfig
doas make menuconfig

doas make clean
doas make bindeb-pkg

# As a result of the build, a custom kernel package linux-image-3.2.19_3.2.19-1_i386.deb (name will reflect the version of the kernel and build number) will be created in the directory one level above the top of the tree. It may be installed using dpkg just as any other package:

#dpkg -i ../linux-image-3.2.19_3.2.19-1_i386.deb
#doas systemctl reboot
