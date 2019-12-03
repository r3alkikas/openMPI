#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

//compile: mpicc mpi_tst.c -o mpi
//run: mpirun -np 3 -v mpi_tst

int main(int argc, char *argv[]){

    int rank, size,i sndcnt=1;
    double reducao, mine;
    char name[100];

MPI_Init (&argc, &argv);
mine = (rank + 1) * size;
MPI_Reduce (&mine, &reducao, sndcnt, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
MPI_Get_processor_name (name, &i);
printf ("P:%d running at %s mine is %f\n", rank, name,mine);
if(rank == 0)
    printf("Root got this from reduction...%g\n", reducao);
MPI_Finalize();
return 0;
}
