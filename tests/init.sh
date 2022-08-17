#!/bin/bash

echo "Init cases"

set -x

cd cases

ls . | parallel -j $(nproc) \
"echo 'Initing' {};
cd {}; \
if [ -e init.sh ]
then
    sh init.sh;
fi
"

cd ../
