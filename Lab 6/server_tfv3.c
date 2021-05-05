//Name: Vincent Yu
//Date: 05/5/2020
//Title: Lab 6
//Description: This program demonstrates TFV3 stop and wait protocol on UDP server side 
    //with unreliable channel and loss


//Explain what each library does in the context of this lab. Do we need all of them?
#include <arpa/inet.h> //This library lets us use htons()
#include <stdio.h> //This library lets us use standard i.o like FILE 
#include <stdlib.h> //This library lets us perform general funcitons like atoi()
#include <string.h> //This library lets us perform string functions
#include <sys/socket.h> //This library lets us declare sockets
#include <time.h> //This library lets us use time functions like rand()
#include <unistd.h> //This library lets us use gethostname() and NULL
#include "lab6.h" //Header file for packet functions

//add the missing functions
//******STUDENT WORK******
int getChecksum(Packet packet) {
    packet.header.cksum = 0;

    int checksum = 0;
    char *ptr = (char *)&packet;
    char *end = ptr + sizeof(Header) + packet.header.len;
    while (ptr < end) {
        checksum ^= *ptr++;
    }

    return checksum;
}

//******STUDENT WORK******
void logPacket(Packet packet) {
    printf("Packet{ header: { seq_ack: %d, len: %d, cksum: %d }, data: \"",
            packet.header.seq_ack,
            packet.header.len,
            packet.header.cksum);
    fwrite(packet.data, (size_t)packet.header.len, 1, stdout);
    printf("\" }\n");
}

void ServerSend(int sockfd, const struct sockaddr *address, socklen_t addrlen, int seqnum) {
    // Simulatting a chance that ACK gets lost
    if (rand() % 5 == 0) {
        //******STUDENT WORK******
        printf("Dropping ACK\n");
        return;
    }
    //prepare and send the ACK
    Packet packet;
    //******STUDENT WORK******
    packet.header.seq_ack = seqnum;
    packet.header.len = sizeof(packet.data);
    packet.header.cksum = getChecksum(packet);
    sendto(sockfd, &packet, sizeof(packet), 0, address, addrlen);

    printf("Sent ACK %d, checksum %d\n", packet.header.seq_ack, packet.header.cksum);
}

Packet ServerReceive(int sockfd, struct sockaddr *address, socklen_t *addrlen, int seqnum)//******STUDENT WORK******
{
                     
    Packet packet;

    while (1) {
        // Receive a packet from the client
        //******STUDENT WORK******
        recvfrom(sockfd, &packet, sizeof(packet), 0, address, addrlen);

        // validate the length of the packet
        //******STUDENT WORK******
        printf("Packet length received: %d\n", packet.header.len);

        // log what was received
        printf("Received: ");
        logPacket(packet);

        //verify the checksum and the sequence number
        if (packet.header.cksum != getChecksum(packet)) {
            printf("Bad checksum, expected %d\n", getChecksum(packet));
            //******STUDENT WORK******
            ServerSend(sockfd, address, *addrlen, !seqnum);
        } else if (packet.header.seq_ack != seqnum) {
            printf("Bad seqnum, expected %d\n", seqnum);
            //******STUDENT WORK******
            ServerSend(sockfd, address, *addrlen, !seqnum);
        } else {
            printf("Good packet\n");
            //******STUDENT WORK******
            ServerSend(sockfd, address, *addrlen, seqnum);
            break;
        }
    }

    printf("\n");
    return packet;
}

int main(int argc, char *argv[]) {
    // check arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <port> <outfile>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // seed the RNG
    srand((unsigned)time(NULL));

    // create a socket
    //******STUDENT WORK******
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Failure to setup an endpoint socket");
        exit(1);
    }

    // initialize the server address structure
    //******STUDENT WORK******
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(atoi(argv[1]));
    address.sin_addr.s_addr = INADDR_ANY;

    // bind the socket
    //******STUDENT WORK******
    if ((bind(sockfd, (struct sockaddr *)&address, sizeof(struct sockaddr))) < 0){
	    perror("Failure to bind server address to the endpoint socket");
        exit(1);
    }

    //Please answer the following questions
        //[Q1] Briefly explain what is "AF_INET" used for.
            //AF_INET is a address family used to designate what types of addresses the socket 
            //can communicate with.
        //[Q2] Briefly explain what is "SOCK_DGRAM" used for.
            //SOCK_DGRAM is a socket datagram protocol that provides messages of fixed length.
        //[Q3] Briefly explain what is "htons" used for.
            //htons() converts unsigned short integer hostshort from host byte order to network byte order.
        //[Q4] Briefly explain why bind is required on the server and not on the client.
            //The client does not need to bind() because it does not have to have a fixed server port number.
    
    
    
    // open file
    FILE *file;
    file = fopen(argv[2], "wb");
    if (file == NULL) {
        perror("fopen");
        return EXIT_FAILURE;
    }
    fclose(file);

    // get file contents from client and save it to the file
    int seqnum = 0;
    Packet packet;
    struct sockaddr_in clientaddr;
    socklen_t clientaddr_len = sizeof(clientaddr);
    do {
        //******STUDENT WORK******
        packet = ServerReceive(sockfd, (struct sockaddr*)&clientaddr, &clientaddr_len, seqnum);
        printf("%s", packet.data);
        FILE *file = fopen(argv[2], "a");
        fwrite(packet.data,1,strlen(packet.data),file);
        fclose(file);
        if (seqnum == 0) {
            seqnum = 1;
        }
        else {
            seqnum = 0;
        }
    } while (packet.header.len != 0);//******STUDENT WORK******

    //cleanup
    //******STUDENT WORK******
    close(sockfd);
    return 0;
}
