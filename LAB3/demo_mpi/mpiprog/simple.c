#include <stdio.h>
#include <string.h>
#include "mpi.h"

/*
  Slide 35
  Compile:
  mpicc -o simple simple.c
  Run:
  mpiexec -f mfile -n 3 ./simple
*/

int main (int argc, char **argv) {
    int myid, numprocs;
    MPI_Init(&argc, &argv); /* initialize MPI */
    MPI_Comm_rank(MPI_COMM_WORLD,&myid); /*rank */
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs); /*no of procs*/
    printf("I am %d of %d.\n", myid, numprocs-1);
    MPI_Finalize(); /* terminate MPI */
    return 0;
}
