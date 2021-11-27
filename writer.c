/*
*   TODO:
*   - threading
*   - logging
*   - cleaning the code
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h> //O_RDONLY
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>

#define MAX_DATA 512

struct fileWritingParams {
    char* pidInfo;
    char* data;
};

int writeToFile (char *pidInfo, char *data) {
//void *writeToFile (void *params) {

    //struct fileWritingParams *givenParams = params; 
    //char *pidInfo = givenParams->pidInfo;
    //char *data = givenParams->data;

    printf("pidInfo: %s, data: %s\n", pidInfo, data);

    /*if (access(pidInfo, F_OK) != -1) {
        printf("fak.\n");
    } else {
        printf("non-fak.\n");
    }*/

    int fd = open(pidInfo, O_CREAT | O_WRONLY);
    printf("fd: %d\n", fd);
    if (fd < 0 || fd == 65) {
        perror("There was an error creating the file");
        //pthread_cancel(params);
        //pthread_exit(params);
    }
    write(fd, data, strlen(data));
    close(fd);
    return 0;
    //pthread_exit(params);
}

//int recieveMessage (char *pidInfo) { //Pid tulee onnistuneesti
void *recieveMessage (void *pidInfo) {
    char path[12];
    char pid[7];
    sprintf(path, "/tmp/%s", (char *)pidInfo);
    path[strlen(path)] = '\0';
    char *fifopath = (char *)malloc(strlen(path)+1);
    strcpy(fifopath, path);
    mkfifo(fifopath, 0666);
    int fifoFD;
    char data[MAX_DATA];

    /*int a = 0;
    while (a < strlen((char *)pidInfo)) {
        pid[a] = ((char *)pidInfo)[a];
        a++;
    }
    printf("pid: %s\n", pid);*/
    //pthread_t thread;
    while(1) {
        fifoFD = open(fifopath, O_RDONLY);
        read(fifoFD, data, sizeof(data));
        //printf("data: %s, datalen: %ld\n", data, strlen(data));
        /*struct fileWritingParams writingParams;
        writingParams.pidInfo = pidInfo;
        writingParams.data = data;
        pthread_create(&thread, NULL, writeToFile, &writingParams);*/
        writeToFile((char *)pidInfo, data);
        //writeToFile(pid, data);
        break;
    }

    free(fifopath);
    unlink(fifopath);
    pthread_exit(pidInfo);
}

int main () {

    char pidInfo[7];
    char *fifofile = "/tmp/metadatafifo";
    mkfifo(fifofile, 0666);
    int pipefd;

    pthread_t tid;
    while (1) {
        pipefd = open(fifofile, O_RDONLY);
        read(pipefd, pidInfo, 7);
        printf("pidINfo: %s\n", pidInfo);
        //recieveMessage(pidInfo);
        pthread_create(&tid, NULL, recieveMessage, (void *)pidInfo);
        //pthread_join(tid, NULL);
        //sleep(1);
        //break;
    }
    return 0;
}