#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <time.h>

#include "logging.c"

#define MAX_DATA 327680 //Defining the maximum data that can be throughputted


struct fileWritingParams { //Custom structure for sending information to function responsbile for filewriting
    char* pidInfo;
    char* data;
};

pthread_mutex_t mtx; //Initializing a mutex

void *writeToFile (void *params) { //Function responsible for writing the data to file

    struct fileWritingParams *givenParams = params; //Extracting the data from the given structure
    char *pidInfo = givenParams->pidInfo;
    char *data = givenParams->data;

    char fileName[18]; //Contains 10-letter timestamp, a '_'-char and pid of the process (max 7char)
    sprintf(fileName, "%ld_%s", time(NULL), pidInfo); //Inserting current time to the variable initialized above
    char fileSuccessfullyCreated[28 + strlen(fileName)];
    sprintf(fileSuccessfullyCreated, "File %s successfully created!", fileName);

    pthread_mutex_lock(&mtx); //Initializing mutex lock for writing the logfile
    int fd = open(fileName, O_CREAT | O_WRONLY); //Creating a new file with the timestamp + pidInfo
    if (fd < 0) { //Checking if creation was successfull
        perror("There was an error creating the file");
        writeLog("There was an error creating a file", 1);
        pthread_exit(params);
    }
    write(fd, data, strlen(data)); //Writing the data to the file
    close(fd); //Closing the file
    writeLog(fileSuccessfullyCreated, 1);
    pthread_mutex_unlock(&mtx); 

    printf("File %s written.\n", fileName); //Printing the name of created file
    pthread_exit(NULL); //Exiting the thread
}

void *recieveMessage (void *pidInfo) { //This function is responsible for reciving the data from the pid-file
    char path[12]; //Initializing path of the pid file
    sprintf(path, "/tmp/%s", (char *)pidInfo); //Setting the value of the pid file
    path[strlen(path)] = '\0';
    mkfifo(path, 0666); //Creating the fifo-connection
    int fifoFD; //Initializing the fifo file description
    char data[MAX_DATA]; //Initializing the data-file

    pthread_mutex_init(&mtx, NULL); //Activating the mutex
    pthread_t thread;
    while(1) { //For some reason requires a while-loop and break, not sure why
        fifoFD = open(path, O_RDONLY); //Opening the fifo file
        read(fifoFD, data, sizeof(data)); //Reading the fifo file to the buffer (data)
        
        struct fileWritingParams writingParams; //Creating a new structure for *writeToFile
        writingParams.pidInfo = (char *)pidInfo; //Setting the pid info
        writingParams.data = data; //Setting the data

        if (strlen(data) > 1000) { //Checking if the sent transmission is "large" (due to see that threading works if there are other smaller transmissions)
            printf("Sleeping a bit, heavy load incoming...\n");
            sleep(5);
        }
        pthread_create(&thread, NULL, writeToFile, &writingParams); //Creating new thread for file writing
        pthread_join(thread, NULL); //Waiting that the thread is finished
        break;
    }

    unlink(path); //Unlinking the fifo file
    pthread_exit(NULL); //Exiting from the thread
}

int main () { //Main function, reads the metadata fifo and recieves the pidInfo which will be used for sending

    char pidInfo[7]; //Initializing the pid, max number is 7
    char *fifofile = "/tmp/metadatafifo"; //Setting the location of metadata fifo
    mkfifo(fifofile, 0666); //Marking the fifo
    int pipefd; //Initializing the fd used for reading the fifo client sends

    pthread_t tid; //Initializing a new thread
    pthread_mutex_init(&mtx, NULL); //Activating the mutex

    while (1) { //Again, for some reason I could not get this working without the while-loop
        
        pipefd = open(fifofile, O_RDONLY); //Opening the metadata file
        read(pipefd, pidInfo, 7); //Receiving the 7 bytes of pid

        pthread_create(&tid, NULL, recieveMessage, (void *)pidInfo); //Creating a new thread
    }
    
    return 0; //never reached because the loop above is supposed to be stopped by ctrl+c
}