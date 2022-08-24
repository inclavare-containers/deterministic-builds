#!/bin/bash

PRELOAD_DIR_PATH="./preload/"
PRELOAD_SO_NAME="modify_time.so"
BPF_DIR_PATH="./bpf/src/"
BPF_Apps=('modify_time' 'modify_file_timestamp' 'modify_file_read' 'preload_filter')

if [[ `whoami` != "root" ]]; then
    echo "Should run in root"
    exit 1
fi

set -x

cd $PRELOAD_DIR_PATH
make
cd ..

cd $BPF_DIR_PATH
make
cd ../..

if [[ ! (-e "${PRELOAD_DIR_PATH}${PRELOAD_SO_NAME}") ]]; then
    echo "${PRELOAD_DIR_PATH}${PRELOAD_SO_NAME} not exist, exit"
    exit 1
fi

for app in "${BPF_Apps[@]}"; do
    echo $app
    if [[ ! (-e "${BPF_DIR_PATH}${app}") ]]; then
        echo "${BPF_DIR_PATH}${app} not exist, exit"
        exit 1
    fi
done

for app in "${BPF_Apps[@]}"; do
    ${BPF_DIR_PATH}${app} &
done

cd ${PRELOAD_DIR_PATH}
./load_modify.sh
cd ..

set +x
