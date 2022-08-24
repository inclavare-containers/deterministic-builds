#!/bin/bash

if [[ `whoami` != "root" ]]; then
    echo "Should run in root"
    exit 1
fi

PRELOAD_PATH=/etc/ld.so.preload 
BACKUP_PATH=/etc/ld.so.preload.backup
SO_NAME=modify_f4ke.so


export LD_PRELOAD=
make
echo "export LD_PRELOAD=$PWD/$SO_NAME"
# export LD_PRELOAD=$PWD/modify_time.so
if [ -e /etc/ld.so.preload  ]; then
    cp $PRELOAD_PATH $BACKUP_PATH
fi
echo "$PWD/$SO_NAME" > /etc/ld.so.preload
echo "testing date"
date
