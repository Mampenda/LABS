#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "mpi.h"

/*
  Slide 57
  Compile:
  mpicc -o barrier barrier.c
  Run:
  mpiexec -f mfile -n 3 ./barrier
*/

int main (int argc, char **argv) {
    int myrank;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&myrank);

    if (myrank == 0) {
        printf("%d: Will sleep 20 seconds.\n", myrank);
        sleep(20);
        printf("%d: My sleep has finished.\n", myrank);
    }
    printf("%d: Before the barrier.\n", myrank);
    MPI_Barrier(MPI_COMM_WORLD);
    printf("%d: After the barrier.\n", myrank);

    MPI_Finalize();
    return 0;
}
