#include<stdio.h>

#define NUM_THREADS_PER_BLOCK 256

__global__
void print_hello()
{   
    int idx = threadIdx.x;
    printf("Hello World! My threadId is %d\n", idx);
}

int main()
{
    print_hello<<<1, NUM_THREADS_PER_BLOCK>>>();
    cudaDeviceSynchronize();
    return 0;
}