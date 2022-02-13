#!/bin/bash

# Environment setup
sudo apt-get update
sudo apt-get install net-tools

# XDP & Kernel Dependency
sudo apt install clang llvm libelf-dev libpcap-dev gcc-multilib build-essential
sudo apt install linux-tools-$(uname -r)
sudo apt install linux-headers-$(uname -r)
sudo apt install linux-tools-common linux-tools-generic
