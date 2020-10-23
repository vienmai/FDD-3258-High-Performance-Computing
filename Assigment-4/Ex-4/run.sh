#!/bin/bash -l 

#SBATCH -J run
#SBATCH -t 00:30:00
#SBATCH -A edu20.FDD3258
#SBATCH --nodes=1
#SBATCH --gres=gpu:K420:1
#SBATCH -e error_file.e
#SBATCH -o output_file.o

for n in 10000 20000 50000 100000 200000 500000
do
	for p in 16 32 64 128 256 
	do  
	    echo "Runing with $n paticles and $p cores."	
	    for i in {1..5}
	    do
	    	srun -n 1 ./particle -n $n -s 100 -p $p | tail -2 | awk 'NR=1{print $4, $9}' >> data/particle_${n}_${p}.txt    
	    done
	done
done