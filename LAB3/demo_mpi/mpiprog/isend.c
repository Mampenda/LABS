#include <stdio.h>
#include <string.h>
#include "mpi.h"

/*
  Slide 59
  Compile:
  mpicc -o isend isend.c
  Run:
  mpiexec -f nfile -n 2 ./isend
*/

int main (int argc, char **argv) {
    int myrank, msgtag=99;
    MPI_Status status;
    MPI_Request request;
    int x;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&myrank); /*rank */
    if (myrank == 0) {
        x=33;
        MPI_Isend(&x,1,MPI_INT,1,msgtag,MPI_COMM_WORLD,&request);
    } else if (myrank == 1) {
        MPI_Irecv(&x,1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&request);
    }

    if (myrank == 0) {
        printf( "%d: I am the master. I have sent an integer \"%d\".\n", myrank, x);
    } else if (myrank == 1) {
        MPI_Wait(&request,&status);
        printf( "%d: I am a slave. I have received an integer \"%d\".\n", myrank, x);
    }

    MPI_Finalize();
    return 0;
}
