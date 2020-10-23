#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/time.h>
#include<math.h>
#include<iostream>

struct Particle{
    float px;
    float py;
    float pz;
    float vx;
    float vy;
    float vz;
};

typedef struct timeval tval;
double get_elapsed(tval t0, tval t1);

void get_input_data(struct Particle **particles, int N, const int seed);
void kernel(struct Particle *particles, int N, int n_steps, const double delta_t, const int seed);

__global__
void kernel_cuda(struct Particle *particles, int N, int n_steps, const double delta_t, const int seed);
__host__ __device__ 
float rand_float(const int seed, int particle, int iter, int N);


#define HLINE "-------------------------------------------------------------\n"

int main(int argc, char *argv[])
{   
    int c;
    int N=1000;
    int n_threads=256; 
    int n_steps=100;
    const int seed=2020;
    const double delta_t=0.05;
    tval t[2] = {0};
    double elapsed[2] = {0};

    while ((c = getopt(argc, argv, "n:p:s:h")) != -1)
        switch (c)
        {
            case 'n':
                N = atoi(optarg);
                break;
            case 'p':
                n_threads = atoi(optarg);
                break;
            case 's':
                n_steps = atoi(optarg);
                break;
            case 'h':
                printf(
                    "Options:\n-n SIZE\t\tNum Particle\n-s ITERS\tNum Iters\n-p NTHREAD\tNumber of threads\n");
                exit(1);
            case '?':
                break;
        }

    struct Particle* particles;
    struct Particle* d_particles;
    get_input_data(&particles, N, seed);
    
    cudaMalloc(&d_particles, N*sizeof(struct Particle));
    
    // For storing the final result copied from Device to Host
    struct Particle* particles_=(struct Particle*)malloc(N*sizeof(struct Particle));
    cudaMemcpy(d_particles, particles, N*sizeof(struct Particle), cudaMemcpyHostToDevice);

    int n_blocks = (N + n_threads - 1) / n_threads;

    printf(HLINE);
    printf("  N\tNum steps\tNum Threads\tNum Blocks\n");
    printf("%4d %8d %14d %16d\n", N, n_steps, n_threads, n_blocks);
    printf(HLINE);
    
    // Launch the CPU version
    printf("Running particle simulation on the CPU...");
    gettimeofday(&t[0], NULL);
    kernel(particles, N, n_steps, delta_t, seed);
    gettimeofday(&t[1], NULL);
    elapsed[0] = get_elapsed(t[0], t[1]);
    printf("Done!\n");

    // Launch the GPU version
    printf("Running particle simulation on the GPU...");
    gettimeofday(&t[0], NULL);
    for (int iter = 1; iter <= n_steps; iter++)
        kernel_cuda<<<n_blocks, n_threads>>>(d_particles, N, iter, delta_t, seed);

    cudaMemcpy(particles_, d_particles, N*sizeof(struct Particle), cudaMemcpyDeviceToHost);
    gettimeofday(&t[1], NULL);    
    elapsed[1] = get_elapsed(t[0], t[1]);
    printf("Done!\n");

    printf("Comparing the output for each implementation...");
    float error = 0.0f;
    for (int i = 0; i < N; i++)
        error = fmax(error, fabs(particles[i].px - particles_[i].px));

    if (error < 1e-8){
        printf("Correct!\n");
    }
    else{
        printf("Not correct!\n");
    }
    printf("Elapsed CPU (ms): %f / Elapsed GPU (ms): %f\n", elapsed[0], elapsed[1]);
    printf(HLINE);

    free(particles);
    free(particles_);
    cudaFree(d_particles);
    return 0;
}

__host__ __device__
float rand_float(const int seed, int particle, int iter, int N)
{
  float result = (seed * particle + iter) % N;
  return result;
}

__global__
void kernel_cuda(struct Particle *particles, int N, int iter, const double delta_t, const int seed)
{
    int q = blockIdx.x * blockDim.x + threadIdx.x;

    if(q < N){
        // particles[q].vx = (rand() / (double)(RAND_MAX)) * 2 - 1;
        // particles[q].vy = (rand() / (double)(RAND_MAX)) * 2 - 1;
        // particles[q].vz = (rand() / (double)(RAND_MAX)) * 2 - 1;
        particles[q].vx = rand_float(seed, q, iter, N) + 0.0f;
        particles[q].vy = rand_float(seed, q, iter, N) + 0.1f;
        particles[q].vz = rand_float(seed, q, iter, N) + 0.2f;

        particles[q].px += delta_t * particles[q].vx;
        particles[q].py += delta_t * particles[q].vy;
        particles[q].pz += delta_t * particles[q].vz;
    }
}


void kernel(struct Particle *particles, int N, int n_steps, const double delta_t, const int seed)
{
    for (int iter = 1; iter <= n_steps; iter++)
    {
        for (int q = 0; q < N; q++)
        {
            // particles[q].vx = (rand() / (double)(RAND_MAX)) * 2 - 1;
            // particles[q].vy = (rand() / (double)(RAND_MAX)) * 2 - 1;
            // particles[q].vz = (rand() / (double)(RAND_MAX)) * 2 - 1;
            particles[q].vx = rand_float(seed, q, iter, N) + 0.0f;
            particles[q].vy = rand_float(seed, q, iter, N) + 0.1f;
            particles[q].vz = rand_float(seed, q, iter, N) + 0.2f;

            particles[q].px += delta_t * particles[q].vx;
            particles[q].py += delta_t * particles[q].vy;
            particles[q].pz += delta_t * particles[q].vz;
        }
    }
}

void get_input_data(struct Particle **particles, int N, const int seed )
{
    struct Particle* particle_t = (struct Particle*)malloc(N*sizeof(struct Particle));
    srand(seed); 

    for(int q=0; q < N; q++)
    {
        particle_t[q].px = (rand() / (double)(RAND_MAX)) * 2 - 1;
        particle_t[q].py = (rand() / (double)(RAND_MAX)) * 2 - 1;
        particle_t[q].pz = (rand() / (double)(RAND_MAX)) * 2 - 1;

        particle_t[q].vx = (rand() / (double)(RAND_MAX)) * 2 - 1;
        particle_t[q].vy = (rand() / (double)(RAND_MAX)) * 2 - 1;
        particle_t[q].vz = (rand() / (double)(RAND_MAX)) * 2 - 1;
    }
    *particles = particle_t;
}

double get_elapsed(tval t0, tval t1)
{
    return (double)(t1.tv_sec - t0.tv_sec) * 1000.0L + (double)(t1.tv_usec - t0.tv_usec) / 1000.0L;
}