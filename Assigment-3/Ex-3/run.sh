#!/bin/bash

srun -n 2 ./ping_pong  >> intra_ping_pong.txt

srun -n 2 -N 2 --ntasks-per-node=1 ./ping_pong  >> inter_ping_pong.txt


    