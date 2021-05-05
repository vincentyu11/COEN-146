//Name: Vincent Yu
//Date: 04/28/2020
//Title: Lab 5
//Description: This program demonstrates TFV2 (rdt2.2 model) stop and wait protocol on UDP client side


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
#include "lab5.h"
#include <netdb.h>



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



void ClientSend(int sockfd, const struct sockaddr *address, socklen_t addrlen, Packet packet) {
    while (1) {

        // send the packet
        printf("Sending packet\n");
		//******STUDENT WORK******
        sendto(sockfd, &packet, sizeof(packet), 0, address, addrlen);

        // receive an ACK from the server
        Packet recvpacket;
		//******STUDENT WORK******
        recvfrom(sockfd, &recvpacket, sizeof(recvpacket), 0, address, &addrlen);

        // log what was received
        printf("Received ACK %d, checksum %d - ", recvpacket.header.seq_ack, recvpacket.header.cksum);

        // validate the ACK
        if (recvpacket.header.cksum != getChecksum(recvpacket)) {	//******STUDENT WORK****** 
            // bad checksum, resend packet
            printf("Bad checksum, expected checksum was: %d\n", getChecksum(recvpacket));//******STUDENT WORK******
        } else if (recvpacket.header.seq_ack != packet.header.seq_ack) {	//******STUDENT WORK******
            // incorrect sequence number, resend packet
            printf("Bad seqnum, expected sequence number was: %d\n", packet.header.seq_ack);	//******STUDENT WORK******
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
    int seqnum = 0;
        fprintf(stderr, "Usage: %s <ip> <port> <infile>\n", argv[0]); //IP: 127.0.0.1
        return 1;
    }

    // seed the RNG
    srand((unsigned)time(NULL));

    // create and configure the socket [name the socket sockfd] 
	//******STUDENT WORK******	 
    int sockfd;   
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Failure to setup an endpoint socket");
        exit(1);
    }

    // initialize the server address structure using argv[2] and argv[1]
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
	//******STUDENT WORK******
    struct hostent *host;
    host = (struct hostent *)gethostbyname(argv[1]);
    address.sin_family = AF_INET;
    address.sin_port = htons(atoi(argv[2]));
    address.sin_addr = *((struct in_addr *)host->h_addr);

    socklen_t addr_len = sizeof(address);

    // open file using argv[3]
	//******STUDENT WORK******
    FILE *in;
    in = fopen(argv[3],"rb");


    // send file contents to server
	//-create a packet
	//-set the seq_ack field 
	//-set the len field
	//-calculate the checksum
	//-send the packet
	//******STUDENT WORK******
    Packet packet;
    int seqnum = 0;
    int n;
    while (n = fread(packet.data, 1, sizeof(packet.data), in) != 0) {
        packet.header.seq_ack = seqnum;
        packet.header.len = sizeof(packet.data);
        packet.header.cksum = getChecksum(packet);
        ClientSend(sockfd, (struct sockaddr*)&address, addr_len, packet);
        if (seqnum == 0) {
            seqnum = 1;
        }
        else {
            seqnum = 0;
        }
    }

    // send zero-length packet to server to end connection
	//******STUDENT WORK******
    Packet zero;
    zero.header.seq_ack = 0;
    zero.header.len = 0;
    zero.header.cksum = getChecksum(zero);
    ClientSend(sockfd, (struct sockaddr*)&address, addr_len, zero);

    // clean up
	//-close the sockets and file
    //******STUDENT WORK******
    fclose(in);
    close(sockfd);


    return 0;
}
