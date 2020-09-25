#include<stdlib.h>
#include<omp.h>
#include<stdio.h>
#include<time.h>
#include<math.h>

#ifndef NTIMES
    #define NTIMES 11
#endif

#ifndef N
    #define N 1000000
#endif

#define HLINE "-------------------------------------------------------------\n"

#ifndef MIN
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#endif
#ifndef MAX
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#endif

int maxloc_critical(double *x);
int maxloc_naive(double *x);
int maxloc_local(double *x);
int maxloc_serial(double *x);
int maxloc_pad(double *x);
void run(int (*f)(double *), double *x);


int main(int argc, char *argv[])
{
    int f_case=0; 
    if (argc > 1){f_case = atoi(argv[1]);};

    // double x[N];
    double *x = (double *)malloc(N * sizeof(double));
    //Initialize arrays
    srand(time(0)); // seed
    // srand(123);
    for (int i = 0; i < N; i++)
    {
        x[i] = ((double)(rand()) / RAND_MAX) * ((double)(rand()) / RAND_MAX) * ((double)(rand()) / RAND_MAX) * 1000;
    }

    switch(f_case)
    {
    case 0:
        run(maxloc_serial, x);
        break;
    case 1:
        run(maxloc_naive, x);
        break;
    case 2:
        run(maxloc_critical, x);
        break;
    case 3:
        run(maxloc_local, x);
        break;
    case 4:
        run(maxloc_pad, x);
        break;
    case '?':
        break;
    }
    
    free(x);
    return 0;
}

void run(int (*f)(double *), double * x)
{
    double avgtime = 0, maxtime = 0, mintime = 0;
    double t, times[NTIMES];
    int mloc, k;
    
    for (k = 0; k < NTIMES; k++)
    {
        t = omp_get_wtime();
        mloc = f(x);
        times[k] = omp_get_wtime() - t;
    }
    printf("Max Index: %d\n", mloc);

    double sumsq = 0;
    for (k = 1; k < NTIMES; k++) // Ignore the first run
    {
        avgtime = avgtime + times[k];
        mintime = MIN(mintime, times[k]);
        maxtime = MAX(maxtime, times[k]);
        sumsq += times[k] * times[k];
    }
    double std = sqrt(sumsq / (double)(NTIMES - 1) - pow(avgtime / (double)(NTIMES - 1), 2));

    printf(HLINE);
    printf("   Avg time     Std time     Min time     Max time\n");
    printf("%11.6f  %11.6f  %11.6f %11.6f\n", avgtime / (double)(NTIMES - 1), std, mintime, maxtime);
    printf(HLINE);
}

int maxloc_serial(double *x)
{
    int maxloc = 0, i;
    double maxval = 0.0;
    
    for (i = 0; i < N; i++)
    {
        if (x[i] > maxval)
        {
            maxval = x[i];
            maxloc = i;
        }
    }
    return maxloc;
}

int maxloc_naive(double *x)
{
    int maxloc=0, i;
    double maxval=0.0;
   
    #pragma omp parallel for
    for (i = 0; i < N; i++)
    {
        if (x[i] > maxval)
        {
            maxval = x[i];
            maxloc = i;
        }
    }
    return maxloc;
}

int maxloc_critical(double *x)
{
    int maxloc = 0, i;
    double maxval = 0.0;

#pragma omp parallel for
    for (i = 0; i < N; i++)
    {
        #pragma omp critical
        if (x[i] > maxval)
        {
            maxval = x[i];
            maxloc = i;
        }
    }
    return maxloc;
}


int maxloc_local(double *x)
{
    const int MAX_THREADS = omp_get_max_threads();
    int thread_id, i;
    double maxval[MAX_THREADS], mval;
    int maxloc[MAX_THREADS], mloc;
    // printf("Max #threads: %d\n", MAX_THREADS);

    #pragma omp parallel private(thread_id) shared(maxval, maxloc)
    {
        thread_id = omp_get_thread_num();
        maxval[thread_id] = -1.0e-30;

        #pragma omp for
        for (i = 0; i < N; i++)
            if (x[i] > maxval[thread_id])
            {
                maxval[thread_id] = x[i];
                maxloc[thread_id] = i;
            }
    }
    mval = maxval[0];
    mloc = maxloc[0];
    for (i = 0; i < MAX_THREADS; i++)
    {
        if (maxval[i] > mval)
        {
            mval = maxval[i];
            mloc = maxloc[i];
        }
    }
    return mloc;
}

int maxloc_pad(double *x)
{
    const int MAX_THREADS = omp_get_max_threads();
    int thread_id, i;
    typedef struct {double val; int loc; char pad[128];} tvals;
    tvals maxinfo[MAX_THREADS];

#pragma omp parallel private(thread_id) shared(maxinfo)
    {
        thread_id = omp_get_thread_num();
        maxinfo[thread_id].val = -1.0e-30;

        #pragma omp for
        for (i = 0; i < N; i++)
            if (x[i] > maxinfo[thread_id].val)
            {
                maxinfo[thread_id].val = x[i];
                maxinfo[thread_id].loc = i;
            }
    }

    double mval = maxinfo[0].val;
    int mloc = maxinfo[0].loc;
    
    for (i = 0; i < MAX_THREADS; i++)
    {
        if (maxinfo[i].val > mval)
        {
            mval = maxinfo[i].val;
            mloc = maxinfo[i].loc;
        }
    }
    return mloc;
}

//gcc  -g -Wall -O2  -fopenmp -o maxloc maxloc.c -lm
//gcc -g -Wall -O2 -I /usr/local/include -Xpreprocessor -fopenmp -o maxloc maxloc.c -L /usr/local/lib -lomp
