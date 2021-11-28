/*
*   TODO:
*   - thread controlling
*   - mutex
*   - cleaning the code
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h> //O_RDONLY
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>

#include <time.h>
#define MAX_DATA 327680

struct fileWritingParams {
    char* pidInfo;
    char* data;
};

pthread_mutex_t mtx;

//int writeToFile (char *pidInfo, char *data) {
void *writeToFile (void *params) {

    struct fileWritingParams *givenParams = params; 
    char *pidInfo = givenParams->pidInfo;
    char *data = givenParams->data;

    //int fd = open(pidInfo, O_CREAT | O_WRONLY);
    char currentTime[10];
    sprintf(currentTime, "%ld", time(NULL));
    printf("currentTime: %s\n", currentTime);
    int fd = open(currentTime, O_CREAT | O_WRONLY);
    if (fd < 0 || fd == 65) {
        printf("file causing error was: %s\n", pidInfo);
        perror("There was an error creating the file");
        //pthread_cancel(params);
        pthread_exit(params);
    }
    printf("strlen(data): %ld\n", strlen(data));
    write(fd, data, strlen(data));
    close(fd);
    /*FILE *fs = fopen(pidInfo, "w");
    fwrite(data, 1, sizeof(data), fs);
    fclose(fs);*/
    //return 0;
    pthread_exit(NULL);
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
    pthread_t thread;
    while(1) {
        fifoFD = open(fifopath, O_RDONLY);
        read(fifoFD, data, sizeof(data));
        printf("pidInfo: %s\n", (char *)pidInfo);
        struct fileWritingParams writingParams;
        writingParams.pidInfo = (char *)pidInfo;
        writingParams.data = data;
        //printf("data: %s\n", data);
        if (strlen(data) > 1000) {
            printf("Sleeping a bit, heavy load incoming...\n");
            sleep(5);
        }

        pthread_mutex_lock(&mtx);
        pthread_create(&thread, NULL, writeToFile, &writingParams);
        pthread_join(thread, NULL);
        pthread_mutex_unlock(&mtx);
        /*int fd = open((char *)pidInfo, O_CREAT | O_WRONLY);
        write(fd, data, strlen(data));
        close(fd);*/

        //writeToFile((char *)pidInfo, data);
        //writeToFile(pid, data);
        break;
    }

    free(fifopath);
    unlink(fifopath);
    pthread_exit(NULL);
}

int main () {

    char pidInfo[7];
    char *fifofile = "/tmp/metadatafifo";
    mkfifo(fifofile, 0666);
    int pipefd, a=1;

    pthread_t tid[100000];
    pthread_mutex_init(&mtx, NULL);

    while (1) {
        
        pipefd = open(fifofile, O_RDONLY);
        read(pipefd, pidInfo, 7);
        //recieveMessage(pidInfo);
        pthread_create(&tid[a], NULL, recieveMessage, (void *)pidInfo);
        a++;
        //tid = tid[a];
        //pthread_join(tid, NULL);
        //sleep(1);
        //break;
    }
    pthread_exit(NULL);
    return 0;
}