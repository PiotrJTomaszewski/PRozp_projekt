#!/bin/bash
rm logs/*
mpiexec -np 2 valgrind --leak-check=full --show-reachable=yes --track-origins=yes --log-file=logs/nc.vg.%p ./nanozombie.out
