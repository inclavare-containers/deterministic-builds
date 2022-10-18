#!/bin/bash

echo "Init cases"

set -ex

cd cases

ls . | parallel -j $(nproc) \
"
if [ -e {}/init.sh ]
then
    echo 'Initing' {}; \
    cd {}; \
    bash init.sh || { err=$?; echo {} failed; exit $err; }
fi"

cd ../
