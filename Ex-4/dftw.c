// the code calculates a DFT of a random complex number input and 
// then an IDFT. The IDFT result should be the input vector 
// to compile with gcc
// gcc -Wall -O2 -fopenmp -o DFTW DFTW.c 
// written by stef

// exercise
#include "stdio.h" // printf
#include "stdlib.h" // malloc and rand for instance. Rand not thread safe!
#include "time.h"   // time(0) to get random seed
#include "math.h"  // sine and cosine
#include "omp.h"   // openmp library like timing

// two pi
#define PI2 6.28318530718
// this for the rounding error, increasing N rounding error increases
// 0.01 precision good for N > 8000
#define R_ERROR 0.01
#ifndef NTIMES
#define NTIMES 11
#endif

#define HLINE "-------------------------------------------------------------\n"

#ifndef MIN
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#endif
#ifndef MAX
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#endif

// main routine to calculate DFT
int DFT(int idft, double* xr, double* xi, double* Xr_o, double* Xi_o, int N);
// set the input array with random number
int fillInput(double* xr, double* xi, int N);
// set to zero the input vector
int setOutputZero(double* Xr_o, double* Xi_o, int N);
// check if x = IDFT(DFT(x))
int checkResults(double* xr, double* xi, double* xr_check, double* xi_check, double* Xr_o, double* Xi_r, int N);
// print the results of the DFT
int printResults(double* xr, double* xi, int N);


int main(int argc, char* argv[]){
    // size of input array
    int N = 10000; // 8,000 is a good number for testing
    printf("DFTW calculation with N = %d \n",N);
    
    // Allocate array for input vector
    double* xr = (double*) malloc (N *sizeof(double));
    double* xi = (double*) malloc (N *sizeof(double));
    fillInput(xr,xi,N);

    // for checking purposes
    double* xr_check = (double*) malloc (N *sizeof(double));
    double* xi_check = (double*) malloc (N *sizeof(double));
    setOutputZero(xr_check,xi_check,N);

    // Allocate array for output vector
    double *Xr_o = (double *)malloc(N * sizeof(double));
    double *Xi_o = (double *)malloc(N * sizeof(double));
    setOutputZero(Xr_o, Xi_o, N);

    double avgtime = 0, maxtime = 0, mintime = 0;
    double t, times[NTIMES];
    int k;

    for (k = 0; k < NTIMES; k++)
    {
        t = omp_get_wtime();
        // DFT
        int idft = 1;
        DFT(idft, xr, xi, Xr_o, Xi_o, N);
        // IDFT
        idft = -1;
        DFT(idft, Xr_o, Xi_o, xr_check, xi_check, N);

        times[k] = omp_get_wtime() - t;

        // check the results: easy to make correctness errors with openMP
        checkResults(xr, xi, xr_check, xi_check, Xr_o, Xi_o, N);
        // print the results of the DFT
        #ifdef DEBUG
                printResults(Xr_o, Xi_o, N);
        #endif

        setOutputZero(Xr_o, Xi_o, N);
        setOutputZero(xr_check, xi_check, N);
    }
   
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
    
    // take out the garbage
    free(xr); free(xi);
    free(Xi_o); free(Xr_o);
    free(xr_check); free(xi_check);

    return 1;
}

// DFT/IDFT routine
// idft: 1 direct DFT, -1 inverse IDFT (Inverse DFT)
int DFT(int idft, double* xr, double* xi, double* Xr_o, double* Xi_o, int N)
{
#pragma omp parallel for //collapse(2)
    for (int k=0 ; k<N ; k++)
        for (int n=0 ; n<N ; n++)  
        {
            // Real part of X[k]
            Xr_o[k] += xr[n] * cos(n * k * PI2 / N) + idft*xi[n]*sin(n * k * PI2 / N);
            // Imaginary part of X[k]
            Xi_o[k] += -idft*xr[n] * sin(n * k * PI2 / N) + xi[n] * cos(n * k * PI2 / N);	            
        } 
    
    // normalize if you are doing IDFT
    if (idft==-1){
        #pragma omp parallel for
        for (int n=0 ; n<N ; n++){
        Xr_o[n] /=N;
        Xi_o[n] /=N;
    }
    }
    return 1; 
}

// set the initial signal 
// be careful with this 
// rand() is NOT thread safe in case
int fillInput(double* xr, double* xi, int N){
    srand(time(0));
    for(int n=0; n < 100000;n++) // get some random number first 
        rand();
    for(int n=0; n < N;n++){
        // Generate random discrete-time signal x in range (-1,+1)
        //xr[n] = ((double)(2.0 * rand()) / RAND_MAX) - 1.0;
        //xi[n] = ((double)(2.0 * rand()) / RAND_MAX) - 1.0;
        // constant real signal
        xr[n] = 1.0;
        xi[n] = 0.0;
    }
    return 1; 
}

// set to zero the output vector
int setOutputZero(double* Xr_o, double* Xi_o, int N)
{
#pragma omp parallel for
    for(int n=0; n < N;n++){
        Xr_o[n] = 0.0;
        Xi_o[n] = 0.0; 
    }
    return 1;
}

// check if x = IDFT(DFT(x))
int checkResults(double* xr, double* xi, double* xr_check, double* xi_check, double* Xr_o, double* Xi_r, int N){
    // x[0] and x[1] have typical rounding error problem
    // interesting there might be a theorem on this
#pragma omp parallel for
    for(int n=0; n < N;n++){
        if (fabs(xr[n] - xr_check[n]) > R_ERROR)
            printf("ERROR - x[%d] = %f, inv(X)[%d]=%f \n",n,xr[n], n,xr_check[n]);
        if (fabs(xi[n] - xi_check[n]) > R_ERROR)
            printf("ERROR - x[%d] = %f, inv(X)[%d]=%f \n",n,xi[n], n,xi_check[n]);
    }
    printf("Xre[0] = %f \n",Xr_o[0]);
    return 1;
}

// print the results of the DFT
int printResults(double* xr, double* xi, int N)
{
#pragma omp parallel for
    for(int n=0; n < N;n++)
            printf("Xre[%d] = %f, Xim[%d] = %f \n", n, xr[n], n, xi[n]);
    // return 1;
    exit(1);
}

//gcc -g -Wall -O2 -I /usr/local/include -Xpreprocessor -fopenmp -o dftw dftw.c -L /usr/local/lib -lomp