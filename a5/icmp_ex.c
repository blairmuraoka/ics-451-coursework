#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/ip.h> //ip hdeader library (must come before ip_icmp.h)
#include <netinet/ip_icmp.h> //icmp header
#include <arpa/inet.h> //internet address library
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>

#include "checksum.h" //my checksum library

#define BUFSIZE 1500 //1500 MTU (so within one frame in layer 2)
#define PROTO_ICMP 1

static int dropped_packets = 0;
static int sent_packets = 0;
static int received_packets = 0;
static struct timeval time_start, time_end;
static char* _arg1;

void exithandler(int sigquit)
{
  gettimeofday(&time_end, 0);
  long elapsedtime = ((time_end.tv_sec - time_start.tv_sec)*1000000 
    + time_end.tv_usec - time_start.tv_usec)/1000;
  float ratio = 100 - (received_packets/sent_packets)*100;
  printf("\n--- %s ping statistics ---\n", _arg1);
  printf("%d packets transmitted, %d packets received, %.1f%% packet loss, time %ldms\n", 
    sent_packets, received_packets, ratio, 
    elapsedtime);
  exit(1);
}

void handler(int signum) //signal handler
{
  alarm(1);
  signal(SIGINT, exithandler);
}

int main(int argc, char * argv[]){
  char sendbuf[BUFSIZE], recvbuf[BUFSIZE], controlbuf[BUFSIZE];
  struct icmp * icmp;
  struct ip * ip;
  int sockfd;
  int packet_len, recv_len, ip_len, data_len;
  struct addrinfo * ai;
  struct iovec iov;
  struct msghdr msg;

  struct timeval timeout;
  struct sockaddr_in sa;
  struct hostent *server;
  struct timeval t0, t1;

  _arg1 = argv[1];
  //Set timeout to 1 second
  timeout.tv_sec = 1;
  timeout.tv_usec = 0;

  //process addr info
  getaddrinfo(argv[1], NULL, NULL, &ai); 
  server = gethostbyname(argv[1]);
  bzero((char *) &sa, sizeof(sa));
  sa.sin_family = AF_INET;
  bcopy((char *) server->h_addr, (char*) &sa.sin_addr.s_addr, server->h_length);
  char host[1024];
  char service[20];
  getnameinfo(&sa, sizeof sa, host, sizeof host, service, sizeof service, 0);
  

  //process destination address
  printf("PING %s\n", ai->ai_canonname ? ai->ai_canonname : argv[1]);

  //Initialize the socket
  if((sockfd = socket(AF_INET, SOCK_RAW, PROTO_ICMP)) < 0){
    perror("socket"); //check for errors
    exit(1);
  }

  //Set timeout
  setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof timeout);
  
  //initiate ICMP header
  int seq_count = 1;

  //start global timer
  gettimeofday(&time_start, 0);
  //Catch Ctrl+C
  signal(SIGALRM,handler); //register handler to handle SIGALRM
  alarm(1);

  while(1) {
    icmp = (struct icmp *) sendbuf; //map to get proper layout
    icmp->icmp_type = ICMP_ECHO; //Do an echoreply
    icmp->icmp_code = 0; 
    icmp->icmp_id = 42;
    icmp->icmp_seq= seq_count;
    icmp->icmp_cksum = 0;

    //compute checksum
    icmp->icmp_cksum = checksum((unsigned short *) icmp, sizeof(struct icmp));

    packet_len = sizeof(struct icmp);
    
    //send the packet
    //start timer
    gettimeofday(&t0, 0);
    sent_packets++;
    if( sendto(sockfd, sendbuf, packet_len, 0, ai->ai_addr, ai->ai_addrlen) < 0){
      perror("sendto");//error check
      exit(1);
    }
    //built msgheader structure for receiving reply
    iov.iov_base = recvbuf;
    iov.iov_len = BUFSIZE;
    
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control=controlbuf;
    msg.msg_controllen=BUFSIZE;
    //recv the reply
    if((recv_len = recvmsg(sockfd, &msg, 0)) < 0){ //could get interupted ??
      dropped_packets++;
      // printf("dropped packet\n");
    } else {
      received_packets++;
      gettimeofday(&t1, 0);
      // printf("%d\n",recv_len);
      
      ip = (struct ip*) recvbuf;
      ip_len = ip->ip_hl << 2; //length of ip header
      
      icmp = (struct icmp *) (recvbuf + ip_len);
      data_len = (recv_len - ip_len);

      long elapsedtime = ((t1.tv_sec - t0.tv_sec)*1000000 + t1.tv_usec - t0.tv_usec)/1000;
      

      printf("%d bytes from %s (%s) icmp_seq=%d ttl=%d time=%ld ms\n", 
        data_len, host, inet_ntoa(ip->ip_src), (unsigned int)icmp->icmp_seq,
        (unsigned int)ip->ip_ttl, elapsedtime);
    }

    seq_count++;
    sleep(1);
  }

  return 0;
}
