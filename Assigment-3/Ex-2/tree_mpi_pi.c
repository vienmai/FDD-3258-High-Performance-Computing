#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include "pi.h"

#define SEED       2020
#define NUM_ITERS  1000000000

int main(int argc, char *argv[])
{   
    MPI_Init(&argc, &argv);
    int size, rank;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    srand(SEED + (rank << 4));

    int loc_num_iters = NUM_ITERS / size; // num_iters for each local process
    int send_every = 1, receive_every = 2;
    int num_rounds = log2(size);  // number of rounds to get the final sum

    double start_time = MPI_Wtime();

    double loc_pi = compute_pi(loc_num_iters);  // each process computes local pi

    /* This implementation only works when the number of processes is a power of 2 */
    for (int i = 0; i < num_rounds; i++)
    {
        if (rank % (2<<i) == send_every)
        {
            int tag = i;
            int dest = rank - send_every;
            int send_ok = MPI_Send(&loc_pi, 1, MPI_DOUBLE, dest, tag, MPI_COMM_WORLD);

            if (send_ok != MPI_SUCCESS)
            {
                printf("%d: Send failure on round %d\n", rank, tag);
            }
        }
        else if (rank % receive_every == 0)
        {
            int tag = i;
            int source = rank + send_every;

            double pirecv;
            MPI_Status status;
            int recv_ok = MPI_Recv(&pirecv, 1, MPI_DOUBLE, source,
                                tag, MPI_COMM_WORLD, &status);

            if (recv_ok != MPI_SUCCESS)
            {
                printf("%d: Receive failure on round %d\n", rank, tag);
            }

            loc_pi = (pirecv + loc_pi) / 2;
        }
        send_every <<= 1;
        receive_every <<= 1;
    }

    double end_time = MPI_Wtime();

    if (rank == 0)
    {   
        printf("\n");
        printf("After %8d samples, average value of pi = %10.8f\n",
               NUM_ITERS, loc_pi);

        printf("\nExact value of pi: 3.1415926535897 \n");
        printf("Elapsed time: %.2lf seconds\n", end_time - start_time);
    }

    MPI_Finalize();

    return 0;
}
