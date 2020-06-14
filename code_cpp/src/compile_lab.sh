#!/bin/sh
cd build && CXX=/usr/lib64/mpi/gcc/openmpi/bin/mpic++ cmake .. && make clean && make -j12
