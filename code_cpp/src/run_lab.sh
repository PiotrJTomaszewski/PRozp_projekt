#!/bin/sh
# Lab machines run docker and MPI while balancing load tries to connect over docker's network and obviously failes, so we're forcing it to use br0
mpirun --np "$1" --hostfile lab_hosts --mca btl_tcp_if_include br0 "$DIR/nanozombie.out"
