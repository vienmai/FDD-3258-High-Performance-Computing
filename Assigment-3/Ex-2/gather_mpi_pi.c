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
    double * pirecv=NULL;

    MPI_Init(&argc, &argv);    
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    srand(SEED + (rank << 4));
    
    int loc_num_iters = NUM_ITERS / size; // num_iters for each local process 
    double start_time = MPI_Wtime();

    double loc_pi = compute_pi(loc_num_iters);

    if (rank == 0)
    {
        pirecv = (double *) malloc(size * sizeof(double));
    }

    MPI_Gather(&loc_pi, 1, MPI_DOUBLE, pirecv, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if(rank == 0)
    {    
        double pisum = 0;
        for (int n = 0; n < size; n++)
        {
            pisum += pirecv[n];
        }
        double pi = pisum / size;

        printf("\n");
        printf("After %8d samples, average value of pi = %10.8f\n",
               NUM_ITERS, pi);
    }

    double end_time = MPI_Wtime();

    if (rank == 0)
    {
        printf("\nExact value of pi: 3.1415926535897 \n");
        printf("Elapsed time: %.2lf seconds\n", end_time - start_time);
        
        free(pirecv);
    }

    MPI_Finalize();

    return 0;
}

