/*
 * Muraoka, Blair
 * ICS 451 Narayan
 * PROGRAM 2
 * myserver.c
 * This program will create a server locally that listens for a connection. When a
 * connection is accepted, it will transmit the an index.html file to the client.
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char * argv[]) 
{
	int ssocket, csocket, portno, cli_len, fd;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;

	if (argc < 2) {
		error("No port specified. Exiting...\n");
	}

	ssocket = socket(AF_INET, SOCK_STREAM, 0);
	if (ssocket < 0) {
		error("ERROR opening socket");
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(ssocket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		error("ERROR on binding");
	}

	listen(ssocket,5);
	printf("Waiting for connection...\n");
	cli_len = sizeof(cli_addr);

	/* Connection acception */
	while (1) {
		csocket = accept(ssocket, (struct sockaddr *) &cli_addr, &cli_len);
		if (csocket < 0) {
			error("ERROR on accept");
		}
		fd = open("index.html", O_RDONLY);
		/* write until eof is found */
		printf("Connection accepted.\n");
		while (1) {
			bzero(buffer, 256);
			int bytes_read = read(fd, buffer, sizeof(buffer));
			if (bytes_read == 0) {
				break;
			}

			if (bytes_read < 0) {
				error("ERROR on read");
			}

			void *p = buffer;
			while (bytes_read > 0) {
				int bytes_written = write(csocket, p, bytes_read);
				if (bytes_written <= 0) {
					error("ERROR on writing");
				}
				bytes_read -= bytes_written;
				p += bytes_written;
			}
		}
		close(csocket);
	}	
	close(ssocket);
	return 0; 
}