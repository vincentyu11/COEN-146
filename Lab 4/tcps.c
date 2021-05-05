//Name: Vincent Yu
//Date: 04/14/2020
//Title: Lab 4- Part 1 tcpc
//Description: This program uses a multithreaded TCP server to receive and copy a file from a TCP client.



//TCP Server
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>

void *connection_handler(void *arg){
   printf("Connection established to client\n");
   int rb;
   char rbuf[1024];
   int newthreadsocket = *(int*)arg;
   while ((rb = recv(newthreadsocket, rbuf, 1024,0))>0){
      printf("File received.\n");
      FILE *out;
      out = fopen("output.txt", "w");
      rbuf[strlen(rbuf)-1] = '\0';
      printf("File contents: %s\n", rbuf);
		
      fwrite(rbuf,1,strlen(rbuf),out);
      fclose(out);
   }
   printf("File copy complete. Closing thread socket.\n");
   close(newthreadsocket);
}



int main(){
 //Declare socket file descriptor. All  Unix  I/O  streams  are  referenced  by  descriptors
 int sockfd, connfd, rb, sin_size;

 //Declare receiving and sending buffers of size 1k bytes
 char sbuf[1024];

 //Declare server address to which to bind for receiving messages and client address to fill in sending address
 struct sockaddr_in servAddr, clienAddr;

 //Open a TCP socket, if successful, returns  a descriptor  associated  with  an endpoint
 if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Failure to setup an endpoint socket");
    exit(1);
 }

 //Setup the server address to bind using socket addressing structure
 servAddr.sin_family = AF_INET;
 servAddr.sin_port = htons(5000);
 servAddr.sin_addr.s_addr = INADDR_ANY;

 //Set address/port of server endpoint for socket socket descriptor
 if ((bind(sockfd, (struct sockaddr *)&servAddr, sizeof(struct sockaddr))) < 0){
    perror("Failure to bind server address to the endpoint socket");
    exit(1);
 }

 printf("Server waiting for client at port 5000\n");

 // Server listening to the socket endpoint, and can queue 5 client requests
 listen(sockfd, 5);
 sin_size = sizeof(struct sockaddr_in);

int client_sock;
int i = 0;
pthread_t threadid[5];
for (i=0; i<5; i++){
   int client_sock;
   client_sock = accept(sockfd, (struct sockaddr *)&clienAddr, (socklen_t *)&sin_size);
   pthread_create(&threadid[i], NULL, connection_handler, (void*)&client_sock);
}


close(sockfd);
return 0;
}
