
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <mpi.h>
#include "pi.h"

#define SEED       2020
#define NUM_ITERS  1000000000
#define NUM_ROUNDS 1

int main(int argc, char *argv[])
{   
    MPI_Init(&argc, &argv);
    
    int size, rank;
    
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    srand(SEED + (rank << 4));

    double avepi = 0.0;
    int loc_num_iters = NUM_ITERS / size; // num_iters for each local process

    double start_time = MPI_Wtime();

    for (int i = 0; i < NUM_ROUNDS; i++)
    {
        double loc_pi = compute_pi(loc_num_iters);

        /* Workers send loc_pi to master */
        /* Message tag is set to the iteration count */
        if (rank != 0)
        {
            int tag = i;
            int send_ok = MPI_Send(&loc_pi, 1, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD);

            if (send_ok != MPI_SUCCESS)
            {
                printf("%d: Send failure on round %d\n", rank, tag);
            }
        }
        else
        {
            /* Master receives messages from all workers */
            int tag = i;
            double pisum = 0;

            for (int n = 1; n < size; n++)
            {
                double pirecv;
                MPI_Status status;
                int recv_ok = MPI_Recv(&pirecv, 1, MPI_DOUBLE, MPI_ANY_SOURCE,
                                  tag, MPI_COMM_WORLD, &status);

                if (recv_ok != MPI_SUCCESS)
                {
                    printf("%d: Receive failure on round %d\n", rank, tag);
                }

                /* Running total of pi */
                pisum += pirecv;
            }

            /* Master calculates the average value of pi for this iteration */
            double pi = (pisum + loc_pi) / size;
          
            /* Master calculates the average value of pi over all iterations */
            avepi = ((avepi * i) + pi) / (i + 1);
            printf("   After %8d samples, average value of pi = %10.8f\n",
                   (loc_num_iters * (i + 1) * size), avepi);
        }
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
