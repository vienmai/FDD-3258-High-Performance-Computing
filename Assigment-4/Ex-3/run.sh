#!/bin/bash -l 

#SBATCH -J run
#SBATCH -t 00:20:00
#SBATCH -A edu20.FDD3258
#SBATCH --nodes=1
#SBATCH --gres=gpu:K420:1
#SBATCH -e error_file.e
#SBATCH -o output_file.o

for n in 10000 20000 50000 100000 200000 500000 1000000 2000000 5000000 10000000
do
    echo "Runing with array of size $n"	
    for i in {1..5}
    do
        srun -n 1 ./saxpy -n $n | tail -1 | awk 'NR=1{print $4, $9}' >> data/saxpy_${n}.txt    
    done
done