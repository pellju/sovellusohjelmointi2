#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define CLIENTLOG "client.log"
#define WRITERLOG "writer.log"


int writeLog (char *text, int option) { //Function used for logging purposes. Includes possibilities to log both writer and client events in different files.
    int fd;
    if (option == 0){ //Checking if wanted to write into client or writer log file.
        fd = open(CLIENTLOG, O_APPEND | O_WRONLY);
    } else if (option == 1){
        fd = open(WRITERLOG, O_APPEND | O_WRONLY);
    } else {
        printf("Unknown issue. Exiting...\n");
        return -1;
    }
    if (fd < 0) {
        perror("Error opening log file");
        return -1;
    }

    write(fd, text, strlen(text));
    write(fd, "\n", 1);
    close(fd);

    return 0;
}