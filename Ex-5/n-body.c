#include<stdlib.h>
#include<omp.h>
#include<stdio.h>
#include<time.h>
#include<math.h>
#include<string.h> /* For memset */

#define G 6.673e-11
#define DIM 2

typedef double vect_t[DIM];

struct Body{
    double px;
    double py;
    double vx;
    double vy;
    double mass;
};

void get_input_data(struct Body **bodies, int N);
void nbody_basic(struct Body *bodies, vect_t *forces, int N, int n_steps, const double delta_t, int verbose, int print_every);
void nbody_reduced(struct Body *bodies, vect_t *forces, int N, int n_steps, const double delta_t, int verbose, int print_every);

#define HLINE "-------------------------------------------------------------\n"

int main(int argc, char *argv[])
{
    int N=1000;
    int run = 0, verbose=0;
    if(argc>1){N = atoi(argv[1]);};
    if (argc>2){run = atoi(argv[2]);};
    if (argc>3){verbose = atoi(argv[3]);};

    int n_steps=100;
    const double delta_t=0.05;

    struct Body* bodies;
    get_input_data(&bodies, N);

    vect_t* forces = (vect_t*)malloc(N*sizeof(vect_t));

    printf(HLINE);
    printf(" N         G           dt     Num steps     Max Threads\n");
    printf("%d  %2e %8.3f %7d %14d\n", N, G, delta_t, n_steps, omp_get_max_threads());
    printf(HLINE);

    double t1 = omp_get_wtime();
    switch (run)
    {
    case 0:
        printf("         Runing the BASIS n-body method.\n");
        printf(HLINE);
        nbody_basic(bodies, forces, N, n_steps, delta_t, verbose, n_steps);
        break;
    case 1:
        printf("         Runing the REDUCED n-body method.\n");
        printf(HLINE);
        nbody_reduced(bodies, forces, N, n_steps, delta_t, verbose, n_steps);
        break;
    case '?':
        break;
    }
    double t2 = omp_get_wtime();

    printf("Runtime: %5.6f\n", t2-t1);
    printf(HLINE);

    free(bodies);
    free(forces);
    return 0;
}

void nbody_reduced(struct Body *bodies, vect_t *forces, int N, int n_steps, const double delta_t, int verbose, int print_every)
{
    double x_diff, y_diff, dist, dist_cubed;
    int step, tid;
    double force_qk[2];
    int max_threads = omp_get_max_threads();
    typedef vect_t vect_t_arr[max_threads];
    vect_t_arr* forces_threads = (vect_t_arr*)malloc(N*sizeof(vect_t_arr));

#pragma omp parallel private(x_diff, y_diff, dist, dist_cubed, step, tid, force_qk) shared(forces_threads)
    {
        tid = omp_get_thread_num();

        for (step = 1; step <= n_steps; step++)
        {
            // Print output
            if (tid == 0)
            {
                if (verbose && (step % print_every == 0))
                {
                    printf("Body        Position                Velocity\n");
                    for (int q = 0; q < N; q++)
                    {
                        printf("%2d %11f %11f %11f %11f\n", q, bodies[q].px, bodies[q].py, bodies[q].vx, bodies[q].vy);
                    }
                    printf(HLINE);
                }
            }

            // forces = memset(forces, 0, N * sizeof(vect_t));

            #pragma omp for
            for (int q = 0; q < N; q++)
            {
                for (int i = 0; i < max_threads; i++)
                {
                    forces_threads[i][q][0] = 0; 
                    forces_threads[i][q][1] = 0;
                }
            }

// Compute total forces
            #pragma omp for schedule(static, 1) 
            for (int q = 0; q < N; q++)
            {
                for (int k = q + 1; k < N; k++)
                {
                    x_diff = bodies[q].px - bodies[k].px;
                    y_diff = bodies[q].py - bodies[k].py;
                    dist = sqrt(x_diff * x_diff + y_diff * y_diff);
                    dist_cubed = dist * dist * dist;

                    force_qk[0] = G * bodies[q].mass * bodies[k].mass / dist_cubed * x_diff;
                    force_qk[1] = G * bodies[q].mass * bodies[k].mass / dist_cubed * y_diff;

                    // forces[q][0] += force_qk[0];
                    // forces[q][1] += force_qk[1];
                    // forces[k][0] -= force_qk[0];
                    // forces[k][1] -= force_qk[1];

                    forces_threads[tid][q][0] += force_qk[0];
                    forces_threads[tid][q][1] += force_qk[1];
                    forces_threads[tid][k][0] -= force_qk[0];
                    forces_threads[tid][k][1] -= force_qk[1];
                }
            }
            
            #pragma omp for 
            for (int q = 0; q < N; q++)
            {
                forces[q][0] = 0; forces[q][1] = 0;
                for (int i = 0; i < max_threads; i++)
                {
                    forces[q][0] += forces_threads[i][q][0];
                    forces[q][1] += forces_threads[i][q][1];
                }       
            }

            // Compute positions and velocities
            #pragma omp for 
            for (int q = 0; q < N; q++)
            {
                bodies[q].px += delta_t * bodies[q].vx;
                bodies[q].py += delta_t * bodies[q].vy;
                bodies[q].vx += delta_t / bodies[q].mass * forces[q][0];
                bodies[q].vy += delta_t / bodies[q].mass * forces[q][1];
            }
        }        
    }
    free(forces_threads);
}

void nbody_basic(struct Body * bodies, vect_t * forces, int N, int n_steps, const double delta_t, int verbose, int print_every)
{
    double x_diff, y_diff, dist, dist_cubed;
    int tid, step;
#pragma omp parallel private(x_diff, y_diff, dist, dist_cubed, step, tid)
    {
        tid = omp_get_thread_num();
        for (step = 1; step <= n_steps; step++)
        {
            // Print output
            if (tid == 0)
            {
                if (verbose && (step % print_every == 0))
                {
                    printf("Body        Position                Velocity\n");
                    for (int q = 0; q < N; q++)
                    {
                        printf("%2d %11f %11f %11f %11f\n", q, bodies[q].px, bodies[q].py, bodies[q].vx, bodies[q].vy);
                    }
                    printf(HLINE);
                }
            }

            #pragma omp for //schedule(static,1)
            for(int q = 0; q < N; q++)
            {
                forces[q][0] = 0; forces[q][1] = 0;
                for (int k = 0; k < N; k++)
                {
                    if (k != q)
                    {
                        x_diff = bodies[q].px - bodies[k].px;
                        y_diff = bodies[q].py - bodies[k].py;
                        dist = sqrt(x_diff*x_diff + y_diff*y_diff);
                        dist_cubed = dist*dist*dist;
                        forces[q][0] -= G * bodies[q].mass * bodies[k].mass / dist_cubed * x_diff;
                        forces[q][1] -= G * bodies[q].mass * bodies[k].mass / dist_cubed * y_diff;
                    }
                }
            }

            #pragma omp for 
            for (int q = 0; q < N; q++)
            {
                bodies[q].px += delta_t * bodies[q].vx;
                bodies[q].py += delta_t * bodies[q].vy;
                bodies[q].vx += delta_t / bodies[q].mass * forces[q][0];
                bodies[q].vy += delta_t / bodies[q].mass * forces[q][1];
            }
        }
    }
}

void get_input_data(struct Body **bodies, int N)
{
    struct Body* bodies_t = (struct Body*)malloc(N*sizeof(struct Body));

    srand(1234); // seed

    for(int q=0; q < N; q++)
    {
        bodies_t[q].px = (rand() / (double)(RAND_MAX)) * 2 - 1;
        bodies_t[q].py = (rand() / (double)(RAND_MAX)) * 2 - 1;

        bodies_t[q].vx = (rand() / (double)(RAND_MAX)) * 2 - 1;
        bodies_t[q].vy = (rand() / (double)(RAND_MAX)) * 2 - 1;
 
        bodies_t[q].mass = fabs((rand() / (double)(RAND_MAX)) * 2 - 1);
    }

    *bodies = bodies_t;
}

//gcc -g -Wall -O2 -I /usr/local/include -Xpreprocessor -fopenmp -o n-body n-body.c -L /usr/local/lib -lomp