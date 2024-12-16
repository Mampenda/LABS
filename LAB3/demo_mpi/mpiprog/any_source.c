#include <stdio.h>
#include <string.h>
#include "mpi.h"

/*
  Slide 43
  Compile:
  mpicc -o any_source any_source.c
  Run:
  mpiexec -f nfile -n 2 ./any_source
*/

int main (int argc, char **argv) {
    int myrank, msgtag=99;
    MPI_Status status;
    int x;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&myrank); /*rank */
    if (myrank == 0) {
        x=33;
        MPI_Send(&x,1,MPI_INT,1,msgtag,MPI_COMM_WORLD);
        printf( "%d: I am the master. I have sent an integer \"%d\".\n", myrank, x);
    } else {
        MPI_Recv(&x,1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
        printf( "%d: I am a slave. I have received an integer \"%d\".\n", myrank, x);
    }
    MPI_Finalize();
    return 0;
}
