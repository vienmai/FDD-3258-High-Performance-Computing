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

    MPI_Init(&argc, &argv);    
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    srand(SEED + (rank << 4));
    
    int loc_num_iters = NUM_ITERS / size; // num_iters for each local process
    double loc_pi;  
    double start_time = MPI_Wtime();

    if (rank != 0)
    {
        loc_pi = compute_pi(loc_num_iters);

        int tag = rank;
        int send_ok = MPI_Send(&loc_pi, 1, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD);

        if (send_ok != MPI_SUCCESS)
        {
            printf("%d: Send failure on round %d\n", rank, tag);
        }
    }
    else
    {
        // Master receives messages from all workers 
        double pirecv[size - 1];
        MPI_Request requests[size - 1];

        // Start communication first to reduce overhead
        for (int n = 1; n < size; n++)
        {
            MPI_Irecv(&pirecv[n-1], 1, MPI_DOUBLE, n, n, MPI_COMM_WORLD, &requests[n-1]);
        }

        // Start computation later
        loc_pi = compute_pi(loc_num_iters);

        MPI_Waitall(size-1, requests, MPI_STATUSES_IGNORE);
        
        double pisum = loc_pi;
        for (int n = 1; n < size; n++)
        {
            pisum += pirecv[n-1];
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
    }

    MPI_Finalize();

    return 0;
}