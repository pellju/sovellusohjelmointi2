#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <errno.h>
#include <sys/types.h>

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

    char data[1000];
    read(fd, &data, 1000);
    data[strlen(data)-1] = '\0';

    printf("Data from the file given: %s\n", data);

    if (mkfifo("fifofile", 0777) == -1) {
        if (errno == EACCES) {
            printf("No permission.\n");
        } else if (errno == EDQUOT) {
            printf("EDQUOT\n");
        } else if (errno == EEXIST) {
            printf("Already exists.\n");
        } else if (errno == ENAMETOOLONG) {
            printf("PATHNAME too long.\n");
        } else if (errno == ENOENT) {
            printf("ENOENT.\n");
        } else if (errno == ENOSPC) {
            printf("ENOSPC\n");
        } else if (errno == ENOTDIR) {
            printf("ENOTDIR");
        } else if (errno == EROFS) {
            printf("Read-only fileysystem.\n");
        }
        perror("Could not create the file");
        return -1;
    } else {
        printf("ok\n");
    }

    int fifofd = open("fifofile1", O_WRONLY);
    write(fifofd, &data, sizeof(data) * strlen(data));
    close(fifofd);
    
    /*
        <insert the piped connection here>
    */

    close(fd);
    return 0;
}