#!/bin/bash

COMMIT=$1
KERNEL=`find /boot -type f -name \*$COMMIT\* | fgrep vmlinux`
KERNEL_VERSION=${KERNEL/\/boot\/vmlinux-/}
echo Kernel: ${KERNEL_VERSION}
sudo grub-reboot "Advanced options for Ubuntu>Ubuntu, with Linux ${KERNEL_VERSION}"
sudo update-grub
sudo reboot
