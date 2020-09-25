#include <omp.h>
#include <stdio.h>


int main()
{
    const int nthreads = 4;
    // Fork a team of threads
#pragma omp parallel num_threads(nthreads)
    // You can use
    // #pragma omp parallel
    // to run with the default number of threads on the node.
    {
        long tid = omp_get_thread_num();

        // Only thread 0 does this
        if (tid == 0)
        {
            int n_threads = omp_get_num_threads();
            printf("[info] Number of threads = %d\n", n_threads);
        }

        // Print the thread ID
        printf("Hello World from thread = %ld\n", tid);

    }
    // All threads join the master thread and terminate


    return 0;
}

//  gcc -Wall -fopenmp -o ...

// echo |cpp -fopenmp -dM |grep -i open

//gcc -Wall -O2 -I /usr/local/include -Xpreprocessor -fopenmp -o name name.c -L /usr/local/lib -lomp
