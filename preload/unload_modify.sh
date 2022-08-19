#!/bin/bash

if [[ `whoami` != "root" ]]; then
    echo "Should run in root"
    exit 1
fi

# export LD_PRELOAD=
echo "" > /etc/ld.so.preload
echo "testing date"
date
