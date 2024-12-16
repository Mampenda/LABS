#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpi.h"

/*
  Slide 47
  Compile:
  mpicc -o scatter scatter.c
  Run:
  mpiexec -f mfile -n 3 ./scatter
*/

int main (int argc, char **argv) {
    const int NUM_ELMS = 5;
    int i, myrank, size, *sendbuf, recvbuf[NUM_ELMS];
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&myrank);

    if (myrank == 0) {
        sendbuf = (int *) malloc(size*NUM_ELMS*sizeof(int));
        for (i=0;i<size*NUM_ELMS;++i) {
            sendbuf[i] = i;
        }
    } else {
        sendbuf = NULL;
    }

    MPI_Scatter(sendbuf,NUM_ELMS,MPI_INT,recvbuf,NUM_ELMS,MPI_INT,0,MPI_COMM_WORLD);
    for (i=0;i<NUM_ELMS;++i) {
        printf("%d: Got data from %d: %d.\n", myrank, 0, recvbuf[i]);
    }
    MPI_Finalize();
    return 0;
}
