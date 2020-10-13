#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include "pi.h"

#define SEED       2020
#define NUM_ITERS  1000000000

int main(int argc, char *argv[])
{
    int size, rank;
    double  pirecv=0.0;

    MPI_Init(&argc, &argv);    
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    srand(SEED + (rank << 4));
    
    int loc_num_iters = NUM_ITERS / size; // num_iters for each local process 

    double start_time = MPI_Wtime();
    double loc_pi = compute_pi(loc_num_iters);
    
    MPI_Reduce(&loc_pi, &pirecv, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    double end_time = MPI_Wtime();

    if(rank == 0)
    {   
        double pi = pirecv / size;

        printf("\n");
        printf("After %8d samples, average value of pi = %10.8f\n",
               NUM_ITERS, pi);
   
        printf("\nExact value of pi: 3.1415926535897 \n");
        printf("Elapsed time: %.2lf seconds\n", end_time - start_time);
    }

    MPI_Finalize();

    return 0;
}