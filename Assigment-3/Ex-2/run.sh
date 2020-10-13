#!/bin/bash -l 

#SBATCH -J run
#SBATCH -t 01:00:00
#SBATCH -A edu20.FDD3258
#SBATCH --nodes=4
#SBATCH --ntasks-per-node=32
#SBATCH -e error_file.e
#SBATCH -o output_file.o

for n in 4 8 16 32 64 128 
do  
    echo "Runing with $n cores."	
    for i in {1..5}
    do
      srun -n $n ./send_recv_mpi_pi | tail -1 | awk '{print $3}' >> data-new/send_recv_mpi_pi_$n.txt    
      srun -n $n ./tree_mpi_pi | tail -1 | awk '{print $3}' >> data-new/tree_mpi_pi_$n.txt    
      srun -n $n ./nonblock_mpi_pi | tail -1 | awk '{print $3}' >> data-new/nonblock_mpi_pi_$n.txt    
      srun -n $n ./gather_mpi_pi | tail -1 | awk '{print $3}' >> data-new/gather_mpi_pi_$n.txt    
      srun -n $n ./reduced_mpi_pi | tail -1 | awk '{print $3}' >> data-new/reduced_mpi_pi_$n.txt    
    done
done
