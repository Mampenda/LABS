#include <stdio.h>
#include <string.h>
#include "mpi.h"

/*
  Slide 20
  Compile:
  mpicc -o hello hello.c
  Run:
  mpiexec -f mfile -n 3 ./hello
  or:
  mpiexec -machinefile mfile -n 3 ./hello
*/

int main (int argc, char **argv) {
    char message[20];
    int i, rank, size, tag=99;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    if(rank == 0) {
        strcpy(message, "Hello, world");
        for (i=1; i<size; ++i) {
            MPI_Send(message,13,MPI_CHAR,i,tag,MPI_COMM_WORLD);
        }
    } else {
        MPI_Recv(message,20,MPI_CHAR,0,tag,MPI_COMM_WORLD,&status);
    }
    printf( "Message from process = %d: %.13s\n", rank,message);
    MPI_Finalize();
    return 0;
}
