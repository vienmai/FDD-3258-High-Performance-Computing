#!/bin/bash

#Serial 
for i in {1..10}
do
  srun -n 1  ./maxloc | awk 'NR==4{print $1}' >> maxloc_serial.txt
done

#Naive omp parallel
export OMP_NUM_THREADS=32
for i in {1..10}
do
  srun -n 1  ./maxloc 1 | awk 'NR==4{print $1}' >> maxloc_naive_$OMP_NUM_THREADS.txt  
done

for n in 1 2 4 8 12 16 20 24 28 32
do
    export OMP_NUM_THREADS=$n
    for i in {1..10}
    do
      srun -n 1  ./maxloc 2 | awk 'NR==4{print $1}' >> maxloc_critical_$n.txt
      srun -n 1  ./maxloc 3 | awk 'NR==4{print $1}' >> maxloc_local_$n.txt
      srun -n 1  ./maxloc 4 | awk 'NR==4{print $1}' >> maxloc_pad_$n.txt      
    done
done

