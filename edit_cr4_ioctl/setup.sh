#!/bin/bash

# /dev/edit_cr4_device 생성 및 권한 설정, 모듈 적재
sudo mknod /dev/edit_cr4_device c 506 0
sudo chmod 666 /dev/edit_cr4_device
sudo insmod edit_cr4_kernel.ko

