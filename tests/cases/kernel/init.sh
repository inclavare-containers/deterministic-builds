#!/bin/bash

usage() { echo "Usage: $0 [-s <kernel url prefix index>] [-v <kernel version>]" 1>&2; exit 1;}

KERNEL_URL_PREFIXES=("https://cdn.kernel.org/pub/linux/kernel/v5.x/" "https://mirrors.aliyun.com/linux-kernel/v5.x/")

while getopts ":s:v:" opt; do
    case $opt in
        s)
        s="$OPTARG"
        (( s == 0 || s == 1)) || usage
        ;;
        v)
        v="$OPTARG"
        ;;
        *)
        usage
        ;;
    esac
done

if [ -z "$s" ]; then
    KERNEL_URL_PREFIX=${KERNEL_URL_PREFIXES[0]}
else
    KERNEL_URL_PREFIX=${KERNEL_URL_PREFIXES[$s]}
fi

if [ -z "$v" ]; then
    KERNEL_VERSION=5.18.9
else
    KERNEL_VERSION=$v
fi

KERNEL_DIR=linux-$KERNEL_VERSION
KERNEL_SRC_TAR_FILE=$KERNEL_DIR.tar
KERNEL_SRC_XZ_FILE=$KERNEL_SRC_TAR_FILE.xz
KERNEL_SIGN_FILE=$KERNEL_SRC_TAR_FILE.sign

set -eux

apt-get install -y git fakeroot build-essential ncurses-dev xz-utils libssl-dev bc flex libelf-dev bison gnupg2

if [ ! -d "$KERNEL_DIR" ]; then
    echo "Downloading linux kernel $KERNEL_VERSION source from" $KERNEL_URL_PREFIX$KERNEL_SRC_XZ_FILE
    if [ ! -f "$KERNEL_SRC_TAR_FILE" ]; then    
        if [ ! -f "$KERNEL_SRC_XZ_FILE" ]; then
            wget -q $KERNEL_URL_PREFIX$KERNEL_SRC_XZ_FILE
        fi
        unxz -v $KERNEL_SRC_XZ_FILE
    fi

    if [ ! -f "$KERNEL_SIGN_FILE" ]; then
        wget -q $KERNEL_URL_PREFIX$KERNEL_SIGN_FILE
    fi
    gpg2 --locate-keys torvalds@kernel.org gregkh@kernel.org
    gpg --verify $KERNEL_SIGN_FILE

    tar -xf $KERNEL_SRC_TAR_FILE
fi

cd $KERNEL_DIR

if [ ! -f ".config" ]; then
    make defconfig
fi
set +x
