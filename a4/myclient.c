/*
 * Muraoka, Blair
 * ICS 451 Narayan
 * PROGRAM 2
 * myclient.c
 * This program will attempt to connect to a host server (localhost) on a user-specified
 * port and a successful connection is made, it will receive a file from the server and
 * save it into a file called downloaded.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define FILENAME "downloaded.html"

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, bytes_read;
    char buffer[256];
    struct sockaddr_in serv_addr;
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

    if (connect(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        error("ERROR connecting\n");
    }

    /* Read data from server */
    printf("Connecting to server...\n");
    bzero(buffer,256);
    FILE* fp = fopen(FILENAME, "w");
    do {
        bzero(buffer, sizeof(buffer));
        bytes_read = read(sockfd, buffer, 255);
        if (bytes_read > 0) {
            fprintf(fp, "%s", buffer);
        }
    }
    while (bytes_read > 0);
    fclose(fp);
    printf("'%s' downloaded successfully. Closing connection...\n", FILENAME);
    return 0;
}