#!/bin/sh
if ! [ -d build ]
then
    mkdir build
fi
cd build && CXX=/bin/mpic++ cmake .. && make clean && make -j4