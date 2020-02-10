#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

// A simple program to test OpenMP functionality
int main()
{
    long n = 1000000000;
    long i;
    double t1, t2, *a;

    a = (double*) malloc(n*sizeof(double));

    for (i = 0; i < n; ++i) {
        a[i] = (double) rand()/RAND_MAX;
    }

    t1 = omp_get_wtime();

    #pragma omp parallel
    {
        #pragma omp for
        for (i = 0; i < n; ++i) {
            a[i] = sqrt(a[i]*a[i]+3.14);
        }
    }

    t2 = omp_get_wtime();

    printf(" OpenMP :%4d\tTime :%20.8e\n", omp_get_max_threads(), t2-t1);

    free(a);

    return 0;
}
