#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "mpi.h"

/*
  Slide 45
  Compile:
  mpicc -o bcast bcast.c
  Run:
  mpiexec -f mfile -n 3 ./bcast
*/

int main (int argc, char **argv) {

    const int HOSTLENGTH=20;
    char hostname[HOSTLENGTH];
    int myrank, result;
    char message[60];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
    if (myrank == 0) {
        strcpy(message,"Hello world");
        result = gethostname(hostname,HOSTLENGTH);
        if (result == 0) {
            strcat(message, ", this is a message from host ");
            strcat(message, hostname);
        }
    }
  
    MPI_Bcast(message,60,MPI_CHAR,0,MPI_COMM_WORLD);
    printf( "Message from %d to %d: %s.\n", 0, myrank, message);
    MPI_Finalize();
    return 0;
}
