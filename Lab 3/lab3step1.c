//Name: Vincent Yu  
//Date: 03/04/2020
//Title: Lab 3 - Step 1 functions
//Description: This program reads a binary/text file and makes it copy of it using functions
//fopen(), fclose(), fread(), and fwrite().


#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    size_t n;
    char buffer[100];
    FILE *in, *out;
    in = fopen(argv[1],"rb");
    out = fopen(argv[2], "wb");
    while ((n=fread(buffer,1,100,in)) != 0) {   //read file
        fwrite(buffer,1,n,out); //write file
    }
    fclose(in);
    fclose(out);
    return 0;
}
