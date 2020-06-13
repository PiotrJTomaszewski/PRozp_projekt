#!/bin/sh
cd build && CXX=/bin/mpic++ cmake .. && make clean && make -j4