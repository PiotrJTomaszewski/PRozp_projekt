#!/bin/bash
mpiexec -np 2 valgrind --leak-check=full --show-reachable=yes --log-file=logs/nc.vg.%p ./nanozombie
