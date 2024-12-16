#include <stdio.h>
#include <string.h>
#include "mpi.h"

/*
  Slide 37
  Compile:
  mpicc -o master_slave master_slave.c
  Run:
  mpiexec -f mfile -n 3 ./master_slave
*/

void master(int rank) {
    printf("%d: I am the master.\n", rank);
}

void slave(int rank) {
    printf("%d: I am a slave.\n", rank);
}

int main (int argc, char **argv) {
    int myrank;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&myrank); /*rank */
    if (myrank == 0) master(myrank);
    else slave(myrank);
    MPI_Finalize();
    return 0;
}
