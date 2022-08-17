#!/bin/bash

cd cases

ls . | parallel -j $(nproc) \
"echo 'compiling' {};
cd {}; \
if [ -e compile.sh ]
then
    sh compile.sh;
else
    make;
    md5sum *.out;
fi
"

cd ../
