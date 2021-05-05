//Name: Vincent Yu
//Date: 04/28/2020
//Title: Lab 5
//Description: This program demonstrates TFV2 (rdt2.2 model) stop and wait protocol on UDP server side


#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include "lab5.h"


// NOTE: beware of sign extension if the checksum is stored in a char, then converted to an int!
int getChecksum(Packet packet) {
    packet.header.cksum = 0;

    int checksum = 0;
    char *ptr = (char *)&packet;
    char *end = ptr + sizeof(Header) + packet.header.len;

//Please find an error from the remaining part getChecksum() function 
//******STUDENT WORK****** 
    while (ptr < end) {
        //changed "ptr" -> "*ptr"
        checksum ^= *ptr++;
    }

    return checksum;
}

void logPacket(Packet packet) {
    printf("Packet{ header: { seq_ack: %d, len: %d, cksum: %d }, data: \"",
            packet.header.seq_ack,
            packet.header.len,
            packet.header.cksum);
    fwrite(packet.data, (size_t)packet.header.len, 1, stdout);
    printf("\" }\n");
}

//Sending a ACK to the client, i.e., letting the client know what was the status of the packet it sent.
void ServerSend(int sockfd, const struct sockaddr *address, socklen_t addrlen, int seqnum) {
    Packet packet;
	//-Prepare the packet headers
	//-send the packet
    //******STUDENT WORK******
    packet.header.seq_ack = seqnum;
    packet.header.len = sizeof(packet.data);
    packet.header.cksum = getChecksum(packet);
    sendto(sockfd, &packet, sizeof(packet), 0, address, addrlen);

    printf("Sent ACK %d, checksum %d\n", packet.header.seq_ack, packet.header.cksum);
}

Packet ServerReceive(int sockfd, struct sockaddr *address, socklen_t *addrlen, int seqnum) {
    Packet packet;

    while (1) {
        //recv packets from the client
		//******STUDENT WORK******
        recvfrom(sockfd, &packet, sizeof(packet), 0, address, addrlen);

        // log what was received
        printf("Received: ");
        logPacket(packet);

        if (packet.header.cksum != getChecksum(packet)) {	//******STUDENT WORK******
            printf("Bad checksum, expected checksum was: %d\n", getChecksum(packet));	//******STUDENT WORK******
            ServerSend(sockfd, address, *addrlen, !seqnum);
        } else if (packet.header.seq_ack != seqnum) {
            printf("Bad seqnum, expected sequence number was:%d\n", seqnum);	//******STUDENT WORK******
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

    // initialize the server address structure using argv[1]
    struct sockaddr_in address;
	//******STUDENT WORK******
    address.sin_family = AF_INET;
    address.sin_port = htons(atoi(argv[1]));
    address.sin_addr.s_addr = INADDR_ANY;

    // bind the socket
	// [Question]: Do we need to bind the socket on the client side? Why?/Why not?
        //No because the client does not need a specific port.
	//******STUDENT WORK******
    if ((bind(sockfd, (struct sockaddr *)&address, sizeof(struct sockaddr))) < 0){
	    perror("Failure to bind server address to the endpoint socket");
        exit(1);
    }
    // open file using argv[2]
	//******STUDENT WORK******
    FILE *file;
    file = fopen(argv[2], "wb");
    fclose(file);
    // get file contents from client
    int seqnum = 0;
    Packet packet;
    struct sockaddr_in clientaddr;
    socklen_t clientaddr_len = sizeof(clientaddr);
    do {
        packet = ServerReceive(sockfd, (struct sockaddr*)&clientaddr, &clientaddr_len, seqnum);//******STUDENT WORK****** 
	    //******STUDENT WORK******
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
    } while (packet.header.len != 0);
    
    
    close(sockfd);
    return 0;
}
