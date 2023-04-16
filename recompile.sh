#!/bin/bash
source /home/user/linux-sgx/linux/installer/bin/sgxsdk/environment
make clean
make SGX_MODE=SIM
