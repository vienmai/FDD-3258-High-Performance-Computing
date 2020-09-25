#!/bin/bash

for n in 1 2 4 8 12 16 20 24 28 32
do
    export OMP_NUM_THREADS=$n
    for i in {1..5}
    do
      srun -n 1  ./stream | grep "Copy" | awk '{print $2}' >> copy_$n.txt
    done
done

