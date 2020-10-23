#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/time.h>
#include<math.h>
#include<iostream>

#ifndef NUM_THREADS_PER_BLOCK
#define NUM_THREADS_PER_BLOCK 256
#endif

typedef struct timeval tval;
double get_elapsed(tval t0, tval t1);

void saxpy(int n, const float a, float *x, float *y)
{
    for (int i=0; i<n; i++)
    {
        y[i] = a*x[i] + y[i];
    }
}

__global__
void saxpy_cuda(int n,const float a, float *x, float *y)
{
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < n) y[i] = a*x[i] + y[i];
}

int main(int argc, char *argv[])
{   
    int           N                     = 10000;
    const float   a                     = 2.0;
    tval          t[2]                  = { 0 };
    double        elapsed[2]            = { 0 };
    float         *x, *y, *d_x, *d_y;

    int c;
    while ((c = getopt(argc, argv, "n:p:h")) != -1)
        switch (c)
        {
            case 'n':
                N = atoi(optarg);
                break;
            // case 'p':
            //     n_threads = atoi(optarg);
            //     break;
            case 'h':
                printf(
                    "Options:\n-n SIZE\t\tNum Particle\n-p NTHREAD\tNumber of threads\n");
                exit(1);
            case '?':
                break;
        }


    int num_blocks = (N + NUM_THREADS_PER_BLOCK - 1) / NUM_THREADS_PER_BLOCK;
    printf("Num_blocks: %2d  Num_threads_per_block: %2d \n", num_blocks, NUM_THREADS_PER_BLOCK);

    x = (float*)malloc(N*sizeof(float));
    y = (float*)malloc(N*sizeof(float));

    for (int i = 0; i < N; i++) 
    {
       x[i] = ((float)rand() / (float)RAND_MAX)*2 -1;
       y[i] = ((float)rand() / (float)RAND_MAX)*2 -1;
    }

    cudaMalloc(&d_x, N*sizeof(float));
    cudaMalloc(&d_y, N*sizeof(float));
    cudaMemcpy(d_x, x, N*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_y, y, N*sizeof(float), cudaMemcpyHostToDevice);

    // Launch the CPU version
    printf("Computing SAXPY on the CPU...");
    gettimeofday(&t[0], NULL);
    saxpy(N, a, x, y);
    gettimeofday(&t[1], NULL);        
    elapsed[0] = get_elapsed(t[0], t[1]);
    printf("Done!\n");

    // Launch the GPU version
    printf("Computing SAXPY on the GPU...");
    gettimeofday(&t[0], NULL);
    saxpy_cuda<<<num_blocks, NUM_THREADS_PER_BLOCK>>>(N, a, d_x, d_y);
    // Copy array y back to HOST and store in array x
    cudaMemcpy(x, d_y, N*sizeof(float), cudaMemcpyDeviceToHost);
    gettimeofday(&t[1], NULL);    
    elapsed[1] = get_elapsed(t[0], t[1]);
    printf("Done!\n");

    printf("Comparing the output for each implementation...");
    float error = 0.0f;
    for (int i = 0; i < N; i++)
        error = fmax(error, fabs(y[i]-x[i]));

    if (error < 1e-8){
        printf("Correct!\n");
    }
    else{
        printf("Not correct!\n");
    }
    printf("Elapsed CPU (ms): %f / Elapsed GPU (ms): %f\n", elapsed[0], elapsed[1]);

    // Free memory
    free(x);
    free(y);
    cudaFree(d_x);
    cudaFree(d_y);

    return 0;
}

double get_elapsed(tval t0, tval t1)
{
    return (double)(t1.tv_sec - t0.tv_sec) * 1000.0L + (double)(t1.tv_usec - t0.tv_usec) / 1000.0L;
}