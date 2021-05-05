//Name: Vincent Yu
//Date: 04/14/2020
//Title: Lab 3- Part 4 udpc
//Description: This program demonstrates reading and transferring a file through UDP client to UDP server

//UDP Client
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

int main(){
 //Declare socket file descriptor. All  Unix  I/O  streams  are  referenced  by  descriptors
 int sockfd; 

 //Declare sending buffer of size 1k bytes
 char sbuf[1024]; 
 
 //Declare server address
 struct sockaddr_in servAddr; 

 //Converts  domain  names   into  numerical  IP  addresses  via  DNS
 struct hostent *host;
 host = (struct hostent *)gethostbyname("localhost"); //Local host runs the server. You may use "127.0.0.1": loopback IP address
 
 //Open a socket, if successful, returns  a descriptor  associated  with  an endpoint 
 if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("Failure to setup an endpoint socket");
    exit(1);
 }

 //Set the server address to send using socket addressing structure
 servAddr.sin_family = AF_INET;
 servAddr.sin_port = htons(5000);
 servAddr.sin_addr = *((struct in_addr *)host->h_addr);

//Comment out which file not being copied.
char filename[100] = "lab3.jpg";
int fileno = 1;
//char filename[100] = "lab3.txt";
//int fileno = 2;
FILE *in;
in = fopen(filename,"rb");
int n;
  
 //Client to send messages to the server continuously using UDP socket 
//while(1){
  //printf("Client: Type a message to send to Server\n");
  //scanf("%s", sbuf);
//}

if (fileno == 1){
    while (!feof(in)){
    int n = fread(sbuf,1,sizeof(sbuf),in);
    sendto(sockfd, sbuf, n, 0, (struct sockaddr *)&servAddr, sizeof(struct sockaddr));
    }
    fclose(in);
}
if (fileno == 2){
    while ((n = fread(sbuf,1,sizeof(sbuf),in)) != 0){
     sendto(sockfd, sbuf, strlen(sbuf), 0, (struct sockaddr *)&servAddr, sizeof(struct sockaddr));
    }
    fclose(in);
}
 return 0;
}
