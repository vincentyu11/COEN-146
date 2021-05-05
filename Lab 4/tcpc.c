//Name: Vincent Yu
//Date: 04/14/2020
//Title: Lab 4- Part 1 tcpc
//Description: This program uses a TCP client to send a file to a multithreaded TCP server.



//TCP Client
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

int main(){
//Declare socket file descriptor. All  Unix  I/O  streams  are  referenced  by  descriptors. nr is a declaration of the number of recieved bytes.
int sockfd, nr;

//Declare sending and recieving buffers of size 1k bytes
char sbuf[1024], rbuf[1024];

//Declare server address to connect to
struct sockaddr_in servAddr;
struct hostent *host;

//Converts  domain  names   into  numerical  IP  addresses  via  DNS
host = (struct hostent *)gethostbyname("localhost");

//Open a socket, if successful, returns  a descriptor  associated  with  an endpoint
 if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Failure to setup an endpoint socket");
    exit(1);
 }

//Set the server address to send using socket addressing structure
servAddr.sin_family = AF_INET;
servAddr.sin_port = htons(5000);
servAddr.sin_addr = *((struct in_addr *)host->h_addr);

//Connect to the server
if (connect(sockfd, (struct sockaddr *)&servAddr, sizeof(struct sockaddr))){
    perror("Failure to connect to the server");
    exit(1);
}

FILE *in;
in = fopen("input.txt", "r");
int n;
 //Client begins to write and read from the server
 
  printf("Client: transferring file\n");
  while((n = fread(sbuf,1,sizeof(sbuf),in)) != 0){
      printf("File contents: %s \n", sbuf);
      send(sockfd, sbuf, 1024,0);
  }
  fclose(in);
 
 printf("File transfer complete. Closing socket.\n");
 //Close socket descriptor
 close(sockfd);
 return 0;
}
