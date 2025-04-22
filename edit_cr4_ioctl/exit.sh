#!/bin/bash

# 커널 모듈 제거 및 장치 파일 삭제
sudo rmmod edit_cr4_kernel
sudo rm -f /dev/edit_cr4_device