#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h> //O_RDONLY
#include <unistd.h>

#include <sys/stat.h>

#define MAX_DATA 512

int writeToFile (char *pidInfo, char *data) {

    int fd = open(pidInfo, O_CREAT | O_WRONLY);
    if (fd < 0) {
        perror("There was an error creating the file");
        return -1;
    }

    write(fd, &data, strlen(data));

    return 0;
}

int recieveMessage (char *pidInfo) { //Pid tulee onnistuneesti
    char path[12];
    sprintf(path, "/tmp/%s", pidInfo);
    path[strlen(path)] = '\0';
    char *fifopath = (char *)malloc(strlen(path)+1);
    strcpy(fifopath, path);
    mkfifo(fifopath, 0666);
    int fifoFD;
    char data[MAX_DATA];

    while(1) {
        fifoFD = open(fifopath, O_RDONLY);
        read(fifoFD, data, sizeof(data));
        printf("data: %s, datalen: %ld\n", data, strlen(data));
        writeToFile(pidInfo, data);
        break;
    }

    free(fifopath);
    return 0;
}

int main () {

    char pidInfo[7];
    char *fifofile = "/tmp/metadatafifo";
    mkfifo(fifofile, 0666);
    int pipefd;

    while (1) {
        sleep(2);
        pipefd = open(fifofile, O_RDONLY);
        read(pipefd, pidInfo, 7);
        printf("pidINfo: %s\n", pidInfo);
        recieveMessage(pidInfo);
        //break;
    }
    return 0;
}