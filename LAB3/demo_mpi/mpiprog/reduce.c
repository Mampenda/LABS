#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpi.h"

/*
  Slide 54
  Compile:
  mpicc -o reduce reduce.c
  Run:
  mpiexec -f mfile -n 3 ./reduce
*/

int main (int argc, char **argv) {
    int result, send, myrank;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&myrank);

    send=myrank*10+1;

    MPI_Reduce(&send,&result,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
    if (myrank == 0) {
        printf("%d: Reduced data: %d.\n", myrank, result);
    }
    MPI_Finalize();
    return 0;
}
