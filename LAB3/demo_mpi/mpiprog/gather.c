#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpi.h"

/*
  Slide 51
  Compile:
  mpicc -o gather gather.c
  Run:
  mpiexec -f mfile -n 3 ./gather
*/

int main (int argc, char **argv) {
    const int NUM_ELMS = 5;
    int i, myrank, size, sendbuf[NUM_ELMS], *recvbuf;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD,&myrank);

    /* Filling data in sendbuf */
    for (i=0;i<NUM_ELMS;++i) {
        sendbuf[i] = i + myrank*NUM_ELMS;
    }

    if (myrank == 0) {
        recvbuf = (int *) malloc(size*NUM_ELMS*sizeof(int));
    } else {
        recvbuf  = NULL;
    }

    //    if (myrank == 1) sleep(5);
    MPI_Gather(sendbuf,NUM_ELMS,MPI_INT,recvbuf,NUM_ELMS,MPI_INT,0,MPI_COMM_WORLD);

    if (myrank == 0) {
        for (i=0;i<size*NUM_ELMS;++i) {
            printf("%d: Got data from a slave: %d.\n", myrank, recvbuf[i]);
        }
    }

    MPI_Finalize();
    return 0;
}
