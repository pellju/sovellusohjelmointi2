/*
*   TODO:
*   - clean the code
*   - figure out why the fifo-transmission requires while-loops
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <errno.h>
#include <sys/types.h>

#include "logging.c"

#define MAX_DATA 512

int transferData (char *pidInfo, char *data, long int datasize){

    char path[13];
    sprintf(path, "/tmp/%s", pidInfo);
    path[12] = '\0';
    
    char *fifopath = (char *)malloc(strlen(path)+1);
    strcpy(fifopath, path);
    mkfifo(fifopath, 0666);
    
    char data1[datasize];
    int a = 0;
    while (a <= datasize){
        data1[a] = data[a];
        a++;
    }

    printf("strlen(data1): %ld, data1: %s\n", strlen(data1), data1);

    int transferFD;
    while(1) {
        sleep(5);
        transferFD = open(fifopath, O_WRONLY);
        write(transferFD, &data1, strlen(data1)+1);
        break;
    }
    free(fifopath);
    unlink(fifopath);
    return 0;
}

int main (int argc, char *argv[]) {
    
    if (argc < 2) { //Checking that if there's less arguments than required
        printf("Not enough arguments. Exiting...\n");
        return -1; //Exiting if not enough arguments
    } else if (argc > 2) {
        printf("More arguments than required. Not a problem, however, only the first argument will be used.\n");
    }
    printf("File wanted to open: %s\n", argv[1]);

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) { //Opening the file given
        printf("There was problem opening the given file %s. Please check the given name.\n", argv[1]);
        
        if (endingLog(0) < 0){
            printf("Error while trying to write ending log.\n");
        }
        
        return -1;
    } else {
        char fileOpened[25+strlen(argv[1])];
        sprintf(fileOpened, "File %s succesfully opened.", argv[1]);
        if (writeLog(fileOpened, 0) < 0) {
            printf("There was an error while writing log. However, continuing the operation.\n");
        }
    }
    
    char pidInfo[7];
    sprintf(pidInfo, "%d", getpid());
    pidInfo[strlen(pidInfo)] = '\0';
    printf("pidInfo: %s\n", pidInfo);

    char data[MAX_DATA];
    read(fd, &data, MAX_DATA);
    printf("datalength: %ld, data: %s\n", strlen(data), data);

    char *fifofile = "/tmp/metadatafifo";
    mkfifo(fifofile, 0666);
    while (1) { //For some reason program does not work if the while-loop is not activated...
        int fifofd = open(fifofile, O_WRONLY);
        write(fifofd, &pidInfo, 7);
        close(fifofd);

        transferData(pidInfo, data, strlen(data));
        break;
    }

    if (endingLog(0) < 0) {
        printf("Some kind of an error occured.\n");
    }

    return 0;
}