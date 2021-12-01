#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <errno.h>
#include <sys/types.h>
#include <time.h>

#include "logging.c" //Library used for logging...

#define MAX_DATA 327680 //Defining the maximum data which can be read from the file

int transferData (char *pidInfo, char *data, long int datasize){ //Function used for transfering the data

    char path[12]; //Initializing the path variable
    sprintf(path, "/tmp/%s", pidInfo); //Setting the path variable
    path[12] = '\0';
    
    char data1[datasize]; //Data has to be copied into a new variable because not accessible through the parameter
    int a = 0;
    while (a <= datasize){
        data1[a] = data[a];
        a++;
    }
    mkfifo(path, 0666);
    printf("strlen: %ld, data which will be sent: %s\n", strlen(data1), data1); //Printing information from the data which will be sent

    int transferFD; //Initializing the fifo used for transfering the data.
    while(1) { //While loop, for some reason does not work without... (runs only once, see break)
        transferFD = open(path, O_CREAT | O_WRONLY); //Opening the fifo, writing data thread
        ssize_t written; 
        written = write(transferFD, &data1, strlen(data1)+1); //Writing data to the path.
        printf("written: %ld (amount of data written into writer in bytes)\n", written); //Printing amount of data written into fifo
        break; //Breaking from this one-time loop
    }
    if (writeLog("Successfully sent data!\n", 0) < 0) { //Checking if logging the successfull file data transfer is ok
        printf("Logging failed for some reason.\n");
    }
    unlink(path); //Unlinking the fifo
    return 0;
}

int main (int argc, char *argv[]) { //Main function of the client, handles file opening
    
    if (argc < 2) { //Checking that if there's less arguments than required
        printf("Not enough arguments. Exiting...\n");
        return -1; //Exiting if not enough arguments
    } else if (argc > 2) {
        printf("More arguments than required. Not a problem, however, only the first argument will be used.\n");
    }
    printf("File wanted to open: %s\n", argv[1]); //Priting the name which is wanted to be opened

    int fd = open(argv[1], O_RDONLY); //Opening the file
    if (fd < 0) { //Opening the file given
        printf("There was problem opening the given file %s. Please check the given name.\n", argv[1]);
        return -1;
    } else {
        char fileOpened[25+strlen(argv[1])];
        sprintf(fileOpened, "File %s succesfully opened.", argv[1]); //Logging that file was opened successfully
        if (writeLog(fileOpened, 0) < 0) { //Checking if logging was ok.
            printf("There was an error while writing log. However, continuing the operation.\n");
        }
    }
    
    char pidInfo[7];
    sprintf(pidInfo, "%d", getpid()); //Getting pid as string
    pidInfo[strlen(pidInfo)] = '\0';

    char data[MAX_DATA];
    read(fd, &data, MAX_DATA); //Reading the data from the given file
    data[strlen(data)] = '\0';
    printf("datalength: %ld, data: %s\n", strlen(data), data);

    char *fifofile = "/tmp/metadatafifo"; 
    mkfifo(fifofile, 0666); //Preparing to transfer the pid to the writer
    while (1) { //For some reason program does not work if the while-loop is not activated...
        int fifofd = open(fifofile, O_WRONLY); //Opening the fifo
        write(fifofd, &pidInfo, 7); //Writing the pid to the writer, pid is max 7 length
        close(fifofd);

        transferData(pidInfo, data, strlen(data)); //Calling the function used for sending the data.
        break; //Breaking the while loop
    }

    return 0;
}