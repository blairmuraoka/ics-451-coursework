/*
 * Muraoka, Blair
 * ICS 451 Narayan
 * PROGRAM 3
 * server_pg3.c
 * This program will create a server locally that listens for a connection. When a
 * connection is accepted, it will simulate the 3 way handshake by reading a header
 * provided by the client and send the appropriate response.
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>

int main(int argc, char * argv[]) 
{
	int sockfd, newsockfd, portno, cli_len, fd;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;

	if (argc < 2) {
		error("No port specified. Exiting...\n");
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		error("ERROR opening socket");
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		error("ERROR on binding");
	}

	listen(sockfd,5);
	printf("Waiting for connection...\n");
	cli_len = sizeof(cli_addr);

	/* Connection acception (keep open) */
	while (1) {
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &cli_len);
		if (newsockfd < 0) {
			error("ERROR on accept");
		}

		/* PG 3 Start*/
		unsigned char header[20];
		unsigned char swap[2];
		unsigned int new_seq = 0;
		unsigned int new_ack = 0;

		/** Receive first transmission (syn) and new seq no **/
		new_seq = recv(newsockfd, &header, sizeof(header), 0);
		printf("\n::Server Received SYN::\n");
		printheader(header);

		// swap ports around
		swap[0] = header[2];
		swap[1] = header[3];
		header[3] = header[1];
		header[2] = header[0];
		header[1] = swap[1];
		header[0] = swap[0];


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
		header[13] = 0x12;

		/** Send second transmission (syn/ack) **/
		send(newsockfd, header, 20, 0);
		printf("\n::Server Sent SYN/ACK ::\n");
		printheader(header);

		/** Receive third transmission (ack) **/
		recv(newsockfd, &header, sizeof(header), 0);
		printf("\n::Server Received ACK::\n");
		printheader(header);

		printf("Closing connection with client.\n");	
		close(newsockfd);
	}	
	close(sockfd);
	return 0; 
}