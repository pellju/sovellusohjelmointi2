#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h> //O_RDONLY
#include <unistd.h>

#include <sys/stat.h>

#define MAX_DATA 8

int writeData () {
    char receivedData[MAX_DATA];
    char *fifofile = "/tmp/fifofile";
    mkfifo(fifofile, 0666);
    int fd = open(fifofile, O_RDONLY);

    read(fd, receivedData, MAX_DATA);
    receivedData[strlen(receivedData)-1] = '\0';
    printf("receivedData: %s\n", receivedData);
    close(fd);

    char newFileName[10];
    sprintf(newFileName, "%d", getpid());
    printf("newFileName: %s\n", newFileName);
    int wfd = open(newFileName, O_CREAT | O_WRONLY, 0664);
    write(wfd, receivedData, MAX_DATA);
    close(wfd);
    
    return 0;
}

int main () {


    while (1) {
        sleep(2);
        writeData();
        break;
    }
    return 0;
}