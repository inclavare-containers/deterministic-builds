#!/bin/bash

PRELOAD_PATH=/etc/ld.so.preload 
BACKUP_PATH=/etc/ld.so.preload.backup

export LD_PRELOAD=
make
echo "export LD_PRELOAD=$PWD/modify_time.so"
# export LD_PRELOAD=$PWD/modify_time.so
if [ -e /etc/ld.so.preload  ]; then
    cp $PRELOAD_PATH $BACKUP_PATH
fi
echo "$PWD/modify_time.so" > /etc/ld.so.preload
echo "testing date"
date
