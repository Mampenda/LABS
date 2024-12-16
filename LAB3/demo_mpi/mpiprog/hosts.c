#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "mpi.h"

/*
  Slide 28
  Compile:
  mpicc -o hosts hosts.c
  Run:
  mpiexec -f mfile -n 3 ./hosts
*/

int main (int argc, char **argv) {
    const int HOSTLENGTH=20;
    char hostname[HOSTLENGTH];
    int rank,result;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    result = gethostname(hostname,HOSTLENGTH);
    if (result == 0 ) {
        printf( "Process %d is running on host %s.\n", rank, hostname);
    } else {
        perror("gethostname()");
    }
    MPI_Finalize();
    return 0;
}
