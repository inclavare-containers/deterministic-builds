#!/bin/bash

if [[ `whoami` != "root" ]]; then
    echo "Should run in root"
    exit 1
fi

EBPF_PROCESS=$(ps aux | grep '[m]odifytime' | awk '{print $2}')

if [[ -z $EBPF_PROCESS ]]; then
    echo "Process about moditytime is empty"
    exit 1
fi

for pid in $EBPF_PROCESS;
do
    echo "killing process $pid"
    kill -2 $pid
done 
