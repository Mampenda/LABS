#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
    // File variable (saved as fp)
    FILE * fp;
    
    // Character variable( saved as line)
    char * line = NULL;
    
    // Size of - size_t can store the maximum size of a theoretically possible object of any type (including array
    size_t len = 0;
    
    // Value of read integer - ssize_t is the same as size_t , but is a signed type. ssize_t is able to represent the number -1
    ssize_t read;

    // path to file
    char filename[40] = "../input/file";

    // The # of the file (file0, file1, file2)
    int rank;
  
    // Check that we get correct # for file
    if (argc <=1 ) {
        printf("Missing argument, an integer in the range 0 to 2.\n");
    } else {

        // atoi() converts numeric strings into an integer value
        rank = atoi(argv[1]);

        /* Format filename as "../input/file" + rank */
        
        // Create space for rank and
        char numchar[2]; 

        // Write rank into char array
        sprintf(numchar,"%d",rank); 
        
        // Appending rank to "filename"
        strcat(filename, numchar);

        // Open file in read mode
        fp = fopen(filename, "r");

        // Check for error when opening file
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

