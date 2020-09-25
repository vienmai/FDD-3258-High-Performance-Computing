#!/bin/bash

for n in 1 2 4 8 12 16 20 24 28 32
do
    export OMP_NUM_THREADS=$n
    srun -n 1 ./dftw | tail -n 3 | awk 'NR==2{print $1, $2}' >> DFTW-data/dftw_$n.txt    
done

