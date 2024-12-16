# Exercise 3 - Using MPI

## Task 1

### An MPI Demonstration

#### Log into mpi1.dat351 remotely and cd into correct folder

```bash
$ ssh -J dat351@eple.hvl.no student1@mpi1.dat351
Enter passphrase for key '/c/Users/amali/.ssh/id_rsa':
(dat351@eple.hvl.no) Verification code:
student1@mpi1.dat351's password:
Last failed login: Mon Dec 16 12:45:07 CET 2024 from 10.0.0.36 on ssh:notty
There was 1 failed login attempt since the last successful login.
Last login: Tue Oct  8 14:18:04 2024 from 10.0.0.36
[student1@mpi1 ~]$ ls -l
total 4
drwxr-xr-x 9 student1 student 92 Nov 19 18:28 dat351
-rw-r--r-- 1 student1 student 64 Aug 30 13:40 me.txt
[student1@mpi1 ~]$ cd dat351
```

#### Create folder and a file with a list of machines

```bash
[student1@mpi1 dat351]$ mkdir mpi
[student1@mpi1 dat351]$ ls -l
total 0
drwxr-xr-x 2 student1 student  58 Nov 19 19:22 cprog
drwxr-xr-x 3 student1 student  33 Nov 19 19:29 htcondor
drwxr-xr-x 2 student1 student 140 Sep 24 14:56 java
drwxr-xr-x 5 student1 student  61 Oct  8 10:10 mpi
drwxr-xr-x 2 student1 student  31 Sep 24 14:56 output
drwxr-xr-x 4 student1 student  37 Nov 19 19:25 slurm
drwxr-xr-x 2 student1 student  68 Nov 19 20:00 torque
[student1@mpi1 dat351]$ cd mpi
[student1@mpi1 mpi]$ cat > machines <<EOF
> mpi1 . dat351
> mpi2 . dat351
> mpi3 . dat351
> EOF
[student1@mpi1 mpi]$ ls -l
total 12
drwxr-xr-x 2 student1 student   42 Sep 24 14:55 input
-rw-r--r-- 1 student1 student   36 Oct  7 13:54 machines
drwxr-xr-x 3 student1 student 4096 Oct  8 12:18 slurmjobs
[student1@mpi1 mpi]$ cat machines
mpi1.dat351
mpi2.dat351
mpi3.dat351
```

#### Create folder for c-programs and copy hello.c into it and compile it

```bash
[student1@mpi1 mpi]$ mkdir cprog
[student1@mpi1 mpi]$ cd cprog
[student1@mpi1 cprog]$ cp /share/dat351/cprog/hello.c .
[student1@mpi1 cprog]$ mpicc -o hello hello .c
[student1@mpi1 cprog]$ ls -l
total 144
-rwxr-xr-x 1 student1 student 28080 Oct  7 13:45 hello
-rw-r--r-- 1 student1 student   638 Sep 26 12:54 hello.c
[student1@mpi1 cprog]$ cat hello.c
#include <stdio.h>
#include <string.h>
#include "mpi.h"

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
    }
    else {
        MPI_Recv(message,20,MPI_CHAR,0,tag,MPI_COMM_WORLD,&status);
    }
    printf( "Message from process = %d: %.13s\n", rank,message);
    MPI_Finalize();
    return 0;
}
```

#### Execute job on three machines

```bash
[student1@mpi1 cprog]$ cd ..
[student1@mpi1 mpi]$ mpiexec -f machines -n 3 ./cprog/hello
Message from process = 1: Hello, world
Message from process = 0: Hello, world
Message from process = 2: Hello, world
```

### Sum numbers from file with C

#### Copy the sum.c file into cprog directory and compile it

```bash
[student1@mpi1 cprog]$ cp /share/dat351/cprog/sum.c .
[student1@mpi1 cprog]$ cc sum.c -lm -o sum
[student1@mpi1 cprog]$ ls -l
total 56
-rwxr-xr-x 1 student1 student 28080 Oct  7 13:45 hello
-rw-r--r-- 1 student1 student   638 Sep 26 12:54 hello.c
drwxr-xr-x 2 student1 student   118 Dec 16 13:10 myCPrograms
-rwxr-xr-x 1 student1 student 18536 Dec 16  2024 sum
-rw-r--r-- 1 student1 student  1126 Dec 16 13:10 sum.c
[student1@mpi1 cprog]$ cat sum.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    char filename[40] = "/share/dat351/input/file";
    int rank;

    if (argc <=1 ) {
        printf("Missing argument, an integer in the range 0 to 2.\n");
    } else {
        rank = atoi(argv[1]);

        # Need filename as "/share/dat351/input/file" + rank
        char numchar[2]; // Need space for rank and \0
        sprintf(numchar,"%d",rank); // Write rank into char array
        strcat(filename,numchar);  // Appending rank to "filename"

        fp = fopen(filename, "r");
        if (fp == NULL) {
            printf("Could not open file \"%s\".\n",filename);
        } else {
            printf("Will sum numbers from file \"%s\".\n",filename);
            double sum=0;
            while ((read = getline(&line, &len, fp)) != -1) {
                if (read > 0) sum += atof(line);
            }
            printf("Sum is %f.\n",sum);

            fclose(fp);
            if (line) free(line);
        }
    }
    return 0;
}
```

#### Write a new program which computes the required statistics (count, largest number, smallest number, and sum of numbers), and compile it

```bash
[student1@mpi1 cprog]$ cat > mySum.c <<EOF
> #include <stdio.h>
> #include <stdlib.h>
> #include <string.h>
> #include <float.h> // For DBL_MAX and DBL_MIN
>
> int main(int argc, char* argv[]) {
>     FILE *fp;
>     char *line = NULL;
>     size_t len = 0;
>     ssize_t read;
>     char filename[50] = "/share/dat351/input/file";
>     int rank;
>
>     // Validate input argument
>     if (argc <= 1) {
>         printf("Missing argument, an integer in the range 0 to 2.\n");
>         return 1;
>     }
>
>     // Convert input argument to rank
>     rank = atoi(argv[1]);
>
>     // Append rank to the filename
>     char numchar[2];
>     sprintf(numchar, "%d", rank);
>     strcat(filename, numchar);
>
>     // Open the file
>     fp = fopen(filename, "r");
>     if (fp == NULL) {
>         printf("Could not open file \"%s\".\n", filename);
>         return 1;
>     }
>
>     printf("Processing file \"%s\".\n", filename);
>
>     // Initialize variables for calculations
>     int count = 0;
>     double sum = 0.0;
>     double largest = DBL_MIN;
>     double smallest = DBL_MAX;
>
>     // Read numbers from file and compute statistics
>     while ((read = getline(&line, &len, fp)) != -1) {
>         if (read > 0) {
>             double num = atof(line);
>             count++;
>             sum += num;
>             if (num > largest) largest = num;
>             if (num < smallest) smallest = num;
>         }
>     }
>
>     // Close the file and free memory
>     fclose(fp);
>     if (line) free(line);
>
>     // Print the results
>     printf("File \"%s\":\n", filename);
>     printf("  Number of elements: %d\n", count);
>     printf("  Sum: %f\n", sum);
>     printf("  Largest number: %f\n", largest);
>     printf("  Smallest number: %f\n", smallest);
>
>     return 0;
> }
> EOF
[student1@mpi1 cprog]$ cc mySum.c -lm -o mysum
[student1@mpi1 cprog]$ ls -l
total 60
-rwxr-xr-x 1 student1 student 28080 Oct  7 13:45 hello
-rw-r--r-- 1 student1 student   638 Sep 26 12:54 hello.c
drwxr-xr-x 2 student1 student   118 Dec 16 13:10 myCPrograms
-rw-r--r-- 1 student1 student  1641 Dec 16  2024 mySum.c
-rwxr-xr-x 1 student1 student 18536 Dec 16 13:15 sum
-rw-r--r-- 1 student1 student  1126 Dec 16 13:10 sum.c
```

#### Execute job on three machines by manually inputting the rank

```bash
[student1@mpi1 mpi]$ mpiexec -f machines -n 1 ./cprog/mySum 0 : -n 1 ./cprog/mySum 1 : -n 1 ./cprog/mySum 2
Processing file "/share/dat351/input/file0".
File "/share/dat351/input/file0":
  Number of elements: 1000000
  Sum: -6500014.163152
  Largest number: 9999.960000
  Smallest number: -9999.980000
Processing file "/share/dat351/input/file1".
File "/share/dat351/input/file1":
  Number of elements: 1000000
  Sum: 317121.111132
  Largest number: 9999.980000
  Smallest number: -9999.990000
Processing file "/share/dat351/input/file2".
File "/share/dat351/input/file2":
  Number of elements: 1000000
  Sum: -8785574.264418
  Largest number: 9999.970000
  Smallest number: -9999.970000
```

This shows that the program now only calculates for the given file (same filenr as jobnr), so to fix this, re-write the program and compile it using the correct compiler

#### Modifications

The exercise asks for aggregate results across all files, which requires combining the results from all processes into a single result. This is a typical use case for MPI reduction operations or manual aggregation through communication.

```bash
[student1@mpi1 cprog]$ rm *#
rm: remove regular file '#mySum.c#'? y
rm: remove regular file '#sum.c#'? y
[student1@mpi1 cprog]$ cat > mySum.c << EOF
> #include <stdio.h>
> #include <stdlib.h>
> #include <string.h>
> #include <float.h> // For DBL_MAX and DBL_MIN
> #include <mpi.h>   // For MPI functions
>
> int main(int argc, char* argv[]) {
>     MPI_Init(&argc, &argv); // Initialize MPI
>
>     int rank, size;
>     MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Get the rank of this process
>     MPI_Comm_size(MPI_COMM_WORLD, &size); // Get the total number of processes
>
>     FILE *fp;
>     char *line = NULL;
>     size_t len = 0;
>     ssize_t read;
>     char filename[50] = "/share/dat351/input/file";
>
>     // Append rank to the filename
>     char numchar[2];
>     sprintf(numchar, "%d", rank);
>     strcat(filename, numchar);
>
>     // Open the file
>     fp = fopen(filename, "r");
>     if (fp == NULL) {
>         printf("Rank %d: Could not open file \"%s\".\n", rank, filename);
>         MPI_Finalize();
>         return 1;
>     }
>
>     // Local variables for statistics
>     int local_count = 0;
>     double local_sum = 0.0;
>     double local_largest = DBL_MIN;
>     double local_smallest = DBL_MAX;
>
>     // Read numbers from file and compute local statistics
>     while ((read = getline(&line, &len, fp)) != -1) {
>         if (read > 0) {
>             double num = atof(line);
>             local_count++;
>             local_sum += num;
>             if (num > local_largest) local_largest = num;
>             if (num < local_smallest) local_smallest = num;
>         }
>     }
>
>     // Close the file and free memory
>     fclose(fp);
>     if (line) free(line);
>
>     // Print local statistics (for debugging)
>     printf("Rank %d - File \"%s\":\n", rank, filename);
>     printf("  Local count: %d\n", local_count);
>     printf("  Local sum: %f\n", local_sum);
>     printf("  Local largest: %f\n", local_largest);
>     printf("  Local smallest: %f\n", local_smallest);
>
>     // Prepare variables for global reduction
>     int global_count = 0;
>     double global_sum = 0.0;
>     double global_largest = DBL_MIN;
>     double global_smallest = DBL_MAX;
>
>     // Perform MPI reductions to calculate global statistics
>     MPI_Reduce(&local_count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
>     MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
>     MPI_Reduce(&local_largest, &global_largest, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
>     MPI_Reduce(&local_smallest, &global_smallest, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
>
>     // Root process prints the global results
>     if (rank == 0) {
>         printf("\nGlobal Results:\n");
>         printf("  Number of elements: %d\n", global_count);
>         printf("  Sum: %f\n", global_sum);
>         printf("  Largest number: %f\n", global_largest);
>         printf("  Smallest number: %f\n", global_smallest);
>     }
>
>     MPI_Finalize(); // Finalize MPI
>     return 0;
> }
> EOF
[student1@mpi1 cprog]$ mpicc mySum.c -lm -o mySum
[student1@mpi1 cprog]$ ls -l
total 92
-rwxr-xr-x 1 student1 student 28080 Oct  7 13:45 hello
-rw-r--r-- 1 student1 student   638 Sep 26 12:54 hello.c
drwxr-xr-x 2 student1 student   118 Dec 16 13:10 myCPrograms
-rwxr-xr-x 1 student1 student 31712 Dec 16 13:46 mySum
-rw-r--r-- 1 student1 student  2742 Dec 16 13:41 mySum.c
-rwxr-xr-x 1 student1 student 18536 Dec 16 13:15 sum
-rw-r--r-- 1 student1 student  1126 Dec 16 13:10 sum.c
[student1@mpi1 cprog]$
```

#### Key-changes for the new program:

_Per-Process Computation_:
Each process calculates its local statistics (local_count, local_sum, local_largest, local_smallest).

_Global Reduction Using MPI_:
MPI_Reduce is used to aggregate results:

- MPI_SUM for count and sum.
- MPI_MAX for largest number.
- MPI_MIN for smallest number.

_Root Process Aggregation_:
Only the root process (rank 0) receives and prints the aggregated results.

#### Re-run the program

```bash
[student1@mpi1 mpi]$ mpiexec -f machines -n 3 ./cprog/mySum
Rank 0 - File "/share/dat351/input/file0":
Local count: 1000000
Local sum: -6500014.163152
Local largest: 9999.960000
Local smallest: -9999.980000
Rank 2 - File "/share/dat351/input/file2":
Local count: 1000000
Local sum: -8785574.264418
Local largest: 9999.970000
Local smallest: -9999.970000
Rank 1 - File "/share/dat351/input/file1":
Local count: 1000000
Local sum: 317121.111132
Local largest: 9999.980000
Local smallest: -9999.990000

Global Results:
Number of elements: 3000000
Sum: -14968467.316438
Largest number: 9999.980000
Smallest number: -9999.990000
```

## Task 2

### A Slurm Demonstration

#### Log into Slurm node and cd into directory

```bash
$ ssh -J dat351@eple.hvl.no student1@slurmmaster.dat351
Enter passphrase for key '/c/Users/amali/.ssh/id_rsa':
(dat351@eple.hvl.no) Verification code:
student1@slurmmaster.dat351's password:
Last login: Tue Nov 19 19:23:51 2024 from 10.0.0.36
[student1@slurmmaster ~]$ ls -l
total 4
drwxr-xr-x 9 student1 student 92 Nov 19 18:28 dat351
-rw-r--r-- 1 student1 student 64 Aug 30 13:40 me.txt
[student1@slurmmaster ~]$ cd dat351/slurm
```

#### Check that mpich is installed

```bash
[student1@slurmmaster mpi_job]$ which mpicc
/usr/lib64/mpich/bin/mpicc
[student1@slurmmaster mpi_job]$ mpicc --version
gcc (GCC) 8.5.0 20210514 (Red Hat 8.5.0-22)
Copyright (C) 2018 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

[student1@slurmmaster mpi_job]$ mpiexec --version
-bash: mpiexec: command not found
[student1@slurmmaster mpi_job]$ dpkg -l | grep mpich
-bash: dpkg: command not found
[student1@slurmmaster mpi_job]$ rpm -qa | grep mpich
mpich-3.4.2-1.slurm.el8.x86_64
python3-mpich-3.4.2-1.slurm.el8.x86_64
mpich-devel-3.4.2-1.slurm.el8.x86_64
```

#### Create new dir for mpi jobs, cd into it and write Slurm job script

```bash
[student1@slurmmaster slurm]$ ls -l
total 0
drwxr-xr-x 2 student1 student 114 Nov 19 19:20 example_job
drwxr-xr-x 2 student1 student 80 Nov 19 19:17 pi_job
[student1@slurmmaster slurm]$ mkdir mpi_job
[student1@slurmmaster slurm]$ cd mpi_job/
[student1@slurmmaster mpi_job]$ cat > hello_mpi << EOF
> #!/bin/bash
> #SBATCH --job-name=my*hello_mpi   # Name of the job
> #SBATCH --output=hello_mpi*%j.txt # Standard output file (%j will be replaced with job ID)
> #SBATCH --ntasks=3                # Number of tasks (processes)
> #SBATCH --nodes=3                 # Use 3 nodes
> #SBATCH --time=00:05:00           # Set a time limit (5 minutes in this example)
>
> # Load the required modules (assuming MPICH with Slurm support is already available)
>
> module load mpich
>
> # Run the MPI program with srun
>
> srun /share/home/student1/dat351/mpi/cprog/hello
> EOF
```

#### Submit the Slurm Job

```bash
[student1@slurmmaster mpi_job]$ sbatch hello_mpi.slurm
Submitted batch job 161700
[student1@slurmmaster mpi_job]$ squeue
             JOBID PARTITION     NAME     USER ST       TIME  NODES NODELIST(REASON)
            161700     batch my_hello student1 PD       0:00      3 (Nodes required for job are DOWN, DRAINED or reserved for jobs in higher priority partitions)
```

The message (Nodes required for job are DOWN, DRAINED or reserved for jobs in higher priority partitions) tells you that the nodes needed for your job are either:

- DOWN: The nodes are not operational.
- DRAINED: The nodes are being taken out of service for maintenance or other reasons.
- Reserved for higher priority jobs: Other jobs with higher priority are using those nodes.

#### Check Status for Slurm Nodes Avaliable

    10.0.0.239 (slurmmaster.dat351) – Slurm server and worker node
    10.0.0.240 (slurmw1.dat351) – Slurm worker node
    10.0.0.241 (slurmw2.dat351) – Slurm worker node

```bash
[student1@slurmmaster mpi_job]$ scontrol show nodes
NodeName=slurmmaster Arch=x86_64 CoresPerSocket=1
   CPUAlloc=0 CPUTot=2 CPULoad=0.08
   AvailableFeatures=(null)
   ActiveFeatures=(null)
   Gres=(null)
   NodeAddr=slurmmaster NodeHostName=slurmmaster Version=20.11.9
   OS=Linux 4.18.0-553.30.1.el8_10.x86_64 #1 SMP Tue Nov 26 18:56:25 UTC 2024
   RealMemory=1 AllocMem=0 FreeMem=246 Sockets=2 Boards=1
   State=IDLE ThreadsPerCore=1 TmpDisk=0 Weight=1 Owner=N/A MCS_label=N/A
   Partitions=batch
   BootTime=2024-12-06T13:07:06 SlurmdStartTime=2024-12-06T13:07:42
   CfgTRES=cpu=2,mem=1M,billing=2
   AllocTRES=
   CapWatts=n/a
   CurrentWatts=0 AveWatts=0
   ExtSensorsJoules=n/s ExtSensorsWatts=0 ExtSensorsTemp=n/s
   Comment=(null)

NodeName=slurmw1 Arch=x86_64 CoresPerSocket=1
   CPUAlloc=0 CPUTot=2 CPULoad=0.00
   AvailableFeatures=(null)
   ActiveFeatures=(null)
   Gres=(null)
   NodeAddr=slurmw1 NodeHostName=slurmw1 Version=20.11.9
   OS=Linux 4.18.0-553.30.1.el8_10.x86_64 #1 SMP Tue Nov 26 18:56:25 UTC 2024
   RealMemory=1 AllocMem=0 FreeMem=545 Sockets=2 Boards=1
   State=IDLE ThreadsPerCore=1 TmpDisk=0 Weight=1 Owner=N/A MCS_label=N/A
   Partitions=batch
   BootTime=2024-12-06T13:07:05 SlurmdStartTime=2024-12-06T13:07:43
   CfgTRES=cpu=2,mem=1M,billing=2
   AllocTRES=
   CapWatts=n/a
   CurrentWatts=0 AveWatts=0
   ExtSensorsJoules=n/s ExtSensorsWatts=0 ExtSensorsTemp=n/s
   Comment=(null)

NodeName=slurmw2 CoresPerSocket=1
   CPUAlloc=0 CPUTot=2 CPULoad=N/A
   AvailableFeatures=(null)
   ActiveFeatures=(null)
   Gres=(null)
   NodeAddr=slurmw2 NodeHostName=slurmw2
   RealMemory=1 AllocMem=0 FreeMem=N/A Sockets=2 Boards=1
   State=DOWN* ThreadsPerCore=1 TmpDisk=0 Weight=1 Owner=N/A MCS_label=N/A
   Partitions=batch
   BootTime=None SlurmdStartTime=None
   CfgTRES=cpu=2,mem=1M,billing=2
   AllocTRES=
   CapWatts=n/a
   CurrentWatts=0 AveWatts=0
   ExtSensorsJoules=n/s ExtSensorsWatts=0 ExtSensorsTemp=n/s
   Reason=Not responding [slurm@2024-12-02T10:32:49]
   Comment=(null)
```

From the output we can see that we have three nodes listed:

    slurmmaster (Slurm server and worker node)
    slurmw1 (worker node)
    slurmw2 (worker node, but it is in a DOWN* state)

`slurmw2` is in a DOWN state and cannot be used for the job.
`slurmmaster` and `slurmw1` are in an IDLE state, which means they are available and can be used for the job, so we check if the job runs with one or two nodes.

#### Rewrite the script to only use two nodes

```bash
[student1@slurmmaster mpi_job]$ cat > hello_mpi << EOF
> #!/bin/bash
> #SBATCH --job-name=my*hello_mpi   # Name of the job
> #SBATCH --output=hello_mpi*%j.txt # Standard output file (%j will be replaced with job ID)
> #SBATCH --ntasks=3                # Number of tasks (processes)
> #SBATCH --nodes=2                 # Use slurm master and worker node
> #SBATCH --time=00:05:00           # Set a time limit (5 minutes in this example)
>
> # Load the required modules (assuming MPICH with Slurm support is already available)
>
> module load mpich
>
> # Run the MPI program with srun
>
> srun /share/home/student1/dat351/mpi/cprog/hello
> EOF
```

```bash
[student1@slurmmaster mpi_job]$ sbatch hello_mpi.slurm
Submitted batch job 161703
[student1@slurmmaster mpi_job]$ squeue
             JOBID PARTITION     NAME     USER ST       TIME  NODES NODELIST(REASON)
[student1@slurmmaster mpi_job]$ ls -l
total 8
-rw-r--r-- 1 student1 student 568 Dec 16 15:12 hello_mpi.slurm
-rw-r--r-- 1 student1 student 473 Dec 16  2024 my_hello_mpi_161703.txt
[student1@slurmmaster mpi_job]$ cat my_hello_mpi_161703.txt
Lmod has detected the following error: The following module(s) are unknown:
"mpich"

Please check the spelling or version number. Also try "module spider ..."
It is also possible your cache file is out-of-date; it may help to try:
  $ module --ignore_cache load "mpich"

Also make sure that all modulefiles written in TCL start with the string
#%Module



Message from process = 0: Hello, world
Message from process = 2: Hello, world
Message from process = 1: Hello, world
```

We get an error which indicates that the job was successfully submitted and ran, but there was an issue with loading the MPICH module.
The "mpich" module error should be resolved by either reloading it with --ignore_cache, verifying its availability, or manually setting the paths if necessary.
The job appears to run successfully with MPI, so even if the module wasn’t loaded properly, the system might have still executed the program with default MPI settings.
