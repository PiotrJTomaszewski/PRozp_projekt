#!/bin/sh
DIR="$PWD/build"
echo "Compiling on localhost"
./compile_local.sh
echo "Deploying"
for MACHINE in "192.168.100.231" "192.168.100.235"
do
    ssh "$MACHINE" " mkdir -p $DIR" || true
    scp "$DIR/nanozombie.out" "$MACHINE":/"$DIR/nanozombie.out"
done
echo "Running"
mpirun --np "$1" --oversubscribe --hostfile piotr_hosts "$DIR/nanozombie.out"
