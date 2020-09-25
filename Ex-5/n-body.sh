#!/bin/bash

# for n in 1 2 4 8 12 16 20 24 28 32
# for N in 500 1000 2000
# do
#     for i in {1..10}
#     do
#         srun -n 1 ./n-body $N 0 | tail -n 3 | awk 'NR==2{print $2}' >> n-body-data/n-body_basic_$N.txt    
#         srun -n 1 ./n-body $N 1 | tail -n 3 | awk 'NR==2{print $2}' >> n-body-data/n-body_reduced_$N.txt    
#     done
# done

for n in 1 2 4 8 12 16 20 24 28 32
do
    export OMP_NUM_THREADS=$n
    for i in {1..10}
    do
        srun -n 1 ./n-body 3000 0 | tail -n 3 | awk 'NR==2{print $2}' >> n-body-data/n-body_basic_$n.txt    
    done
done
