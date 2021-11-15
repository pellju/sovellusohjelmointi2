#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main (int argc, char *argv[]) {
    
    if (argc < 2) {
        printf("Not enough arguments. Exiting...\n");
        return -1;
    } else if (argc > 2) {
        printf("More arguments than required. Not a problem, however, the first argument will only be used.\n");
    }
    printf("File wanted to open: %s\n", argv[1]);

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        printf("There was problem opening the given file %s. Please check the given name.\n", argv[1]);
        return -1;
    }

    char data[1000];
    read(fd, &data, 1000);
    data[strlen(data)-1] = '\0';
    
    printf("Data from the file given: %s\n", data);

    close(fd);
    return 0;
}