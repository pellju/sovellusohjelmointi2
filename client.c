#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <errno.h>
#include <sys/types.h>

#define MAX_DATA 512

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
        return -1;
    }

    char data[MAX_DATA];
    read(fd, &data, MAX_DATA);
    data[strlen(data)-1] = '\0';

    printf("Data from the file given: %s\n", data);

    char *fifofile = "/tmp/fifofile";
    mkfifo(fifofile, 0666);
    while (1) {
        sleep(2);
        printf("data: %s\n", data);
        int fifofd = open(fifofile, O_WRONLY);
        write(fifofd, &data, MAX_DATA);
        close(fifofd);
        break;
    }
    /*
        <insert the piped connection here>
    */

    close(fd);
    return 0;
}