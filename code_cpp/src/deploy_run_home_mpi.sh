#!/bin/sh
echo "Compiling on localhost"
make clean || true
./compile.sh &
echo "Deploying"
for MACHINE in "192.168.100.235" "192.168.100.236" "192.168.100.153"
do
	cat .deploy.sh | ssh "$MACHINE" &
done
wait
echo "Running"
mpirun --np 10 --hostfile home_hosts /home/piotr/Documents/studia/rok_III/sem_6/PRozp/PRozp_projekt/code_cpp/src/nanozombie.out
