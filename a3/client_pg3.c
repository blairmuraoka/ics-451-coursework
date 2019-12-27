/*
 * Muraoka, Blair
 * ICS 451 Narayan
 * PROGRAM 3
 * client_pg3.c
 * This program will attempt to connect to a host server (localhost) on a user-specified
 * port and a successful connection is made, it will simulate the 3-way handshake by 
 * sending a 20 byte header to the host and respond appropriately.
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <math.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <time.h>

int main(int argc, char *argv[])
{
    int sockfd, portno, bytes_read;
    char buffer[256];
    struct sockaddr_in serv_addr;
    socklen_t len = sizeof(serv_addr);
    struct hostent *server;

    if (argc != 2) {
       fprintf(stderr,"usage %s port \n", argv[0]);
       exit(1);
    }
    
    portno = atoi(argv[1]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket\n");

    server = gethostbyname("localhost");

    if (server == NULL) {
        error("ERROR, no such host\n");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    printf("Connecting to server...\n");
    if (connect(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        error("ERROR connecting\n");
    }

    /** PG 3 Start **/
    unsigned char header[20];
    unsigned char swap[2];
    unsigned int new_seq = 0;
    unsigned int new_ack = 0;

    // defined in iofunctions_pg3.c
    initialize_header(header, sockfd, atoi(argv[1]));

    /** Send first transmission (syn) **/
    send(sockfd, &header, sizeof(header), 0);
    printf("\n::Client Sent SYN ::\n");
    printheader(header);
    
    /** Receive second transmission (syn/ack) **/
    recv(sockfd, &header, sizeof(header), 0);
    printf("\n::Client Received SYN/ACK::\n");
    printheader(header);

    // swap ports around
    swap[0] = header[2];
    swap[1] = header[3];
    header[3] = header[1];
    header[2] = header[0];
    header[1] = swap[1];
    header[0] = swap[0];

    // new sequence number
    new_seq += (header[8] << 24) + (header[9] << 16) + (header[10] << 8) + header[11];

    // new acknowledgment number (previous seq + 1)
    new_ack += (header[4] << 24) + (header[5] << 16) + (header[6] << 8) + header[7] + 1;

    // set new sequence
    header[4]  = new_seq >> 24;
    header[5]  = new_seq >> 16;
    header[6]  = new_seq >>  8;
    header[7]  = new_seq;

    // set new ack
    header[8]  = new_ack >> 24;
    header[9]  = new_ack >> 16;
    header[10] = new_ack >>  8;
    header[11] = new_ack;

    // set flags to ack/syn = 1
    header[13] = 0x10;

    /** Send third transmission (ack) **/
    send(sockfd, header, 20, 0);
    printf("\n::Client Sent ACK ::\n");
    printheader(header);

    printf("\nClosing connection...\n");
    close(sockfd);

    return 0;
}
