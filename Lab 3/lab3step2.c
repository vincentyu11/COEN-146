//Name: Vincent Yu  
//Date: 03/04/2020
//Title: Lab 3 - Step 1 system calls
//Description: This program reads a binary/text file and makes it copy of it using system calls
//open(), close(), read(), and write().


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
    char buffer[1];
    int in = open(argv[1], O_RDONLY | O_CREAT);
    int out = open(argv[2], O_RDWR | O_CREAT, 0666);
    int nr;
    int nw;
    while (nr = read(in, buffer, sizeof(buffer)) != 0) {
        nw = write(out, buffer, sizeof(buffer));
    }
    close(in);
    close(out);

    return 0;
}