#!/bin/bash

set -x

cd ./preload
make
cd ..

cd ./bpf/src/
make
cd ../..

if [[ ! (-e ./preload/modify_time.so) ]]; then
    echo "./preload/modify_time.so not exist, exit"
    exit 1
fi

if [[ ! (-e ./bpf/src/modify_time) ]]; then
    echo "./bpf/src/modify_time not exist, exit"
    exit 1
fi

# export LD_PRELOAD=$PWD/preload/modify_time.so
./bpf/src/modify_time &

set +x
