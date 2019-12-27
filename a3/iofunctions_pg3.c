/*
 * Muraoka, Blair
 * ICS 451 Narayan
 * PROGRAM 3
 * io_functions_pg3.c
 * This file contains printheader function that is used often in the client_pg3.c
 * and server_pg3.c files. It also initializes the header for the start of the connection.
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

void error(char *msg)
{
    perror(msg);
    exit(1);
}

void initialize_header(unsigned char* header, int fd, int dest_port) 
{
    unsigned int  source_port;
    unsigned int  destination_port;
    unsigned int  seq_no;
    unsigned int  ack_no;
    unsigned int  window_size;
    srand(time(NULL));

    ack_no = 20;
    seq_no = rand();
    window_size = 17520;
    // Source port
    struct sockaddr_in local_address;
    int addr_size = sizeof(local_address);
    if (getsockname(fd, (struct sockaddr *)&local_address, &addr_size) == -1) 
        error("getsockname\n"); //
    else
        source_port = ntohs(local_address.sin_port); // source port

    // Destination port
    destination_port = dest_port; 

    // source port
    header[0]  = source_port >> 8;
    header[1]  = source_port;

    // destination port
    header[2]  = destination_port >> 8;
    header[3]  = destination_port;

    // sequence no
    header[4]  = seq_no >> 24;
    header[5]  = seq_no >> 16;
    header[6]  = seq_no >>  8;
    header[7]  = seq_no;

    // acknowledgement no
    header[8]  = ack_no >> 24;
    header[9]  = ack_no >> 16;
    header[10] = ack_no >>  8;
    header[11] = ack_no;

    // data offset/reserved
    header[12] = 0x00;

    // control flags
    header[13] = 0x02; // syn flag checked

    // window size
    header[14] = window_size >> 8;
    header[15] = window_size;

    // checksum
    header[16] = 0xff;
    header[17] = 0xff;

    // urgent
    header[18] = 0x00;
    header[19] = 0x00;
}

int printheader(unsigned char * header) 
{
    printf("Source port: %u\n", (header[0] << 8) + header[1]);
    printf("Destination port: %u\n", (header[2] << 8) + header[3]);
    printf("Sequence number: %u\n", (header[4] << 24) + (header[5] << 16) + (header[6] << 8) + header[7]);
    printf("Acknowledgement number: %u\n", (header[8] << 24) + (header[9] << 16) + (header[10] << 8) + header[11]);
    printf("Data Offset: %u\n", header[12] >> 4);
    printf("Reserved: %u\n", header[12] >> 4 & 0x0f);
    if (header[13] == 0x02) 
        printf("Control flags: SYN\n");
    else if (header[13] == 0x12) 
        printf("Control flags: SYN ACK\n");
    else if (header[13] == 0x10)
        printf("Control flags: ACK\n");
    
    printf("Window size: %u\n", (header[14] << 8) + header[15]);
    printf("Checksum: %u\n", (header[16] << 8) + header[17]);
    printf("Urgent pointer: %u\n\n", (header[18] << 8) + header[19]);
}
