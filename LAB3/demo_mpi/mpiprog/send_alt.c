#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "mpi.h"

/*
  Slide 58
  Compile:
  mpicc -o ssend ssend.c
  Run:
  mpiexec -f nfile -n 2 ./ssend
*/

int main (int argc, char **argv) {
    int myrank, msgtag=99;
    MPI_Status status;
    int x=33,y;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&myrank); /*rank */
    if (myrank == 0) {

        printf( "%d: I will send an integer to 1.\n", myrank);
        MPI_Send(&x,1,MPI_INT,1,msgtag,MPI_COMM_WORLD);
        printf( "%d: The integer \"%d\" is sent to 1.\n", myrank, x);

        printf( "%d: Sleeping for 20 seconds.\n", myrank);
        sleep(20);

        printf( "%d: I will now receive an integer.\n", myrank);
        MPI_Recv(&y,1,MPI_INT,1,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
        printf( "%d: I have received an integer \"%d\".\n", myrank, y);

    } else if (myrank == 1) {
      
        printf( "%d: I will now receive an integer.\n", myrank);
        MPI_Recv(&y,1,MPI_INT,0,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
        printf( "%d: I have received an integer \"%d\".\n", myrank, y);

        printf( "%d: I will send an integer to 0.\n", myrank);
        MPI_Send(&x,1,MPI_INT,0,msgtag,MPI_COMM_WORLD);
        printf( "%d: The integer \"%d\" is sent to 0.\n", myrank, x);
    }
    MPI_Finalize();
    return 0;
}
