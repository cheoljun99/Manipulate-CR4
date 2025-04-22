#!/bin/bash

# /dev/comms_device 생성 및 권한 설정

sudo mknod /dev/comms_device c 506 0
sudo chmod 666 /dev/comms_device

