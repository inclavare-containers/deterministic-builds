#!/bin/bash

dirs=`ls -d ./linux-*/`

if [ -z "$dirs" ]; then
    exit 1
fi

src_directory=${dirs[0]}

version=$(echo $src_directory | cut -d '/' -f 2 | awk '{ match($0, /([0-9.]+)/); { print substr( $0, RSTART, RLENGTH )} }')

echo "Building linux-"$version

set -x

cd $src_directory

if [ -f ".config" ]; then
    mv .config ../config-linux-$version
    make mrproper
fi

time=$(date +%Y%m%d%H%M%S)
build_dir_base_name=build_$time

counter=0
while true
do
    if [ $counter -ne 0 ]; then
        build_dir="${build_dir_base_name}_$counter"
    else
        build_dir=$build_dir_base_name
    fi
    if [ ! -e $build_dir ]; then
        break
    fi
    ((counter++))
done

mkdir $build_dir

if [ -f "../config-linux-$version" ]; then
    cp ../config-linux-$version $build_dir/.config
else
    exit 1
fi

make -j $(nproc) O=$build_dir

md5sum $build_dir/vmlinux
