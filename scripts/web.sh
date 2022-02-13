#!/bin/bash

# Environment setup
sudo apt-get update
sudo apt-get install net-tools

# XDP & Kernel Dependency
sudo apt-get install clang llvm libelf-dev libpcap-dev gcc-multilib build-essential
sudo apt-get install linux-tools-$(uname -r)
sudo apt-get install linux-headers-$(uname -r)
sudo apt-get install linux-tools-common linux-tools-generic
