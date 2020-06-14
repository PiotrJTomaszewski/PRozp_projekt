#!/bin/bash
mpirun -np "$1" --oversubscribe ./build/nanozombie.out
