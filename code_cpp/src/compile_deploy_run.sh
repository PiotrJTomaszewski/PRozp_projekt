#!/bin/sh
DIR="$PWD/build"
echo "Compiling on localhost"
./compile.sh
echo "Deploying"
for MACHINE in "192.168.100.231" "192.168.100.235"
do
    ssh "$MACHINE" " mkdir -p $DIR" || true
    scp "$DIR/nanozombie.out" "$MACHINE":/"$DIR/nanozombie.out"
done
echo "Running"
mpirun --np 10 --oversubscribe --hostfile home_hosts "$DIR/nanozombie.out"
