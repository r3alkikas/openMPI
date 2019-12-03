#include <math.h>
#include "mpi.h"
#include <cstdlib>
#include <iostream>

//run: /usr/lib/openmpi/1.4-gcc/bin/mpirun -np 4 mpi_PI.exe

int main(int argc, char* argv[]){

    int n, rank, size;
    double PI = 3.141592653589793238462643;
    double mypi, pi, h, sum;
    bool valor_por_parametros = true;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0){
        cout<<"Introduza a precisão do calculo (n>0)";
        cin>>n;
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (n <= 0){
        MPI_Finalize();
        exit(0);
    }
    else{
        h = 1.0 / (double) n;
        sum = 0.0;
        for (int i = rank +1; i<=n; i += size){
            double x = h * ((double)i - 0.5);
            sum += (4.0 / (1.0 + x*x));
        }
        mypi = h * sum;
        MPI_Reduce(&mypi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

        if (rank == 0)
            cout<< "O valor aproximado de PI e: " << pi << ", com um erro de " << fabs(pi - PI25DT) << endl;
    }
    MPI_Finalize();
    return 0;
}
