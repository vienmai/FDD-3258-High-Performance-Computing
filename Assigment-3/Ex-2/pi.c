#include "pi.h"

double compute_pi(int max_iters)
{
    int count = 0;
    double x, y, z;

    for (int iter = 0; iter < max_iters; iter++)
    {
        // Generate random (X,Y) points
        x = (double)random() / (double)RAND_MAX;
        y = (double)random() / (double)RAND_MAX;
        z = sqrt((x * x) + (y * y));

        // Check if point is in unit circle
        if (z <= 1.0)
        {
            count++;
        }
    }
    return 4.0 * ((double)count / (double)max_iters);
}