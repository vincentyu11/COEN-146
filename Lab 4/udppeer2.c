//Name: Vincent Yu
//Date: 04/14/2020
//Title: Lab 3- Part 4 udpc peer2
//Description: This program demonstrates a P2P UDP client/server that can send and receive files.


//UDP P2P
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

int main(){
 //Declare socket file descriptor. All  Unix  I/O  streams  are  referenced  by  descriptors
 int sockfd;

 //Declare sending buffer of size 1k bytes
 char sbuf[1024], rbuf[1024];
 
 //Declare server address
 struct sockaddr_in servAddr1, servAddr2, clienAddr;
 socklen_t addrLen = sizeof(struct sockaddr);

 //Converts  domain  names   into  numerical  IP  addresses  via  DNS
 struct hostent *host;
 host = (struct hostent *)gethostbyname("localhost"); //Local host runs the server. You may use "127.0.0.1": loopback IP address

 //Open a socket, if successful, returns  a descriptor  associated  with  an endpoint 
 if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("Failure to setup an endpoint socket");
    exit(1);
 }

 //Set the server address to send using socket addressing structure
 //bind to this. receive
 servAddr1.sin_family = AF_INET;
 servAddr1.sin_port = htons(6000);
 servAddr1.sin_addr.s_addr = INADDR_ANY; 

 //send
 servAddr2.sin_family = AF_INET;
 servAddr2.sin_port = htons(5000);
 servAddr2.sin_addr = *((struct in_addr *)host->h_addr);

if ((bind(sockfd, (struct sockaddr *)&servAddr1, sizeof(struct sockaddr))) < 0){
	perror("Failure to bind server address to the endpoint socket");
    exit(1);
}

FILE *in, *out;
in = fopen("peer2in.txt","rb");
out = fopen("peer2out.txt", "wb");
int nr;
int n;
  
//Client to send messages to the server continuously using UDP socket 

while (1){
char sr[1];
printf("Send or receive file? (s/r)\n");
scanf("%s",sr);

if (sr[0] == 'r'){
  while(nr = recvfrom(sockfd, rbuf, 1024, 0, (struct sockaddr *)&clienAddr, &addrLen) > 0){
    if (nr > 0){
      rbuf[strlen(rbuf)-1] = '\0';
      fwrite(rbuf,1,strlen(rbuf),out);
      fclose(out);
    }
  break;
  }
}

if (sr[0] == 's'){
  while ((n = fread(sbuf,1,sizeof(sbuf),in)) != 0){
    if (n > 0){
      sendto(sockfd, sbuf, strlen(sbuf), 0, (struct sockaddr *)&servAddr2, sizeof(struct sockaddr));
    }
  break;
  }
  fclose(in);
}
}
 return 0;
}
