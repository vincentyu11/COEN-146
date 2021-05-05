//Name: Vincent Yu
//Date: 05/5/2020
//Title: Lab 6
//Description: This program demonstrates TFV3 stop and wait protocol on UDP client side 
    //with unreliable channel and loss


#include <arpa/inet.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <netdb.h>
#include "lab6.h"

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

void logPacket(Packet packet) {
    printf("Packet{ header: { seq_ack: %d, len: %d, cksum: %d }, data: \"",
            packet.header.seq_ack,
            packet.header.len,
            packet.header.cksum);
    fwrite(packet.data, (size_t)packet.header.len, 1, stdout);
    printf("\" }\n");
}



void ClientSend(int sockfd, const struct sockaddr *address, socklen_t addrlen, Packet packet, unsigned retries) {

    while (1) {
        // calculate the checksum
        //******STUDENT WORK******
        getChecksum(packet);

        // log what is being sent
        printf("Created: ");
        logPacket(packet);

        // Simulate lost packet.
        if (rand() % 5 == 0) {
            printf("Dropping packet\n");
        } else {
            //******STUDENT WORK******
            sendto(sockfd, &packet, sizeof(packet), 0, address, addrlen);
        }

        // wait until either a packet is received or timeout, i.e using the select statement
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        fd_set readfds;
        //******STUDENT WORK******
        int rv;
        fcntl(sockfd, F_SETFL, O_NONBLOCK);
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);

        tv.tv_sec = 10;
        tv.tv_usec = 0;

        rv = select(sockfd+1, &readfds, NULL, NULL, &tv);

        if (rv == 0) {
            sendto(sockfd, &packet, sizeof(packet), 0, address, addrlen);
        }
        else if (rv == 1) {
            printf("Data to be received.\n");
        }
    
        // receive an ACK from the server
        Packet recvpacket;
        //******STUDENT WORK******
        recvfrom(sockfd, &recvpacket, sizeof(recvpacket), 0, address, &addrlen);

        // log what was received
        printf("Received ACK %d, checksum %d - ", recvpacket.header.seq_ack, recvpacket.header.cksum);

        // validate the ACK
        if (recvpacket.header.cksum != getChecksum(recvpacket)) {   //******STUDENT WORK******
            // bad checksum, resend packet
            printf("Bad checksum, expected %d\n", getChecksum(recvpacket));
        } else if (recvpacket.header.seq_ack != packet.header.seq_ack) {     //******STUDENT WORK******
            // incorrect sequence number, resend packet
            printf("Bad seqnum, expected %d\n", packet.header.seq_ack);
        } else {
            // good ACK, we're done
            printf("Good ACK\n");
            break;
        }
    }

    printf("\n");
}

int main(int argc, char *argv[]) {
    // check arguments
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <ip> <port> <infile>\n", argv[0]);
        return 1;
    }

    // seed the RNG
    srand((unsigned)time(NULL));

    // create and configure the socket
    //******STUDENT WORK******
    int sockfd;   
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Failure to setup an endpoint socket");
        exit(1);
    }

    // initialize the server address structure
    //******STUDENT WORK******
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    struct hostent *host;
    host = (struct hostent *)gethostbyname(argv[1]);
    address.sin_family = AF_INET;
    address.sin_port = htons(atoi(argv[2]));
    address.sin_addr = *((struct in_addr *)host->h_addr);

    socklen_t addr_len = sizeof(address);

    // open file
    FILE *file = fopen(argv[3], "rb");
    if (file == NULL) {
        perror("fopen");
        return 1;
    }

    // send file contents to server
    Packet packet;
    //******STUDENT WORK******
    int seqnum = 0;
    int n;
    while (n = fread(packet.data, 1, sizeof(packet.data), file) != 0) {
        packet.header.seq_ack = seqnum;
        packet.header.len = sizeof(packet.data);
        packet.header.cksum = getChecksum(packet);
        ClientSend(sockfd, (struct sockaddr*)&address, addr_len, packet, 0);
        if (seqnum == 0) {
            seqnum = 1;
        }
        else {
            seqnum = 0;
        }
    }

    // send zero-length packet to server to end connection according the description provided in the Lab manual
    //******STUDENT WORK******
    Packet zero;
    zero.header.seq_ack = 0;
    zero.header.len = 0;
    zero.header.cksum = getChecksum(zero);
    ClientSend(sockfd, (struct sockaddr*)&address, addr_len, zero, 0);

    // clean up
    //******STUDENT WORK******
    fclose(file);
    close(sockfd);

    return 0;
}
