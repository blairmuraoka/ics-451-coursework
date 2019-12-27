/* timer.c */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

static int numAlarms = 0;

void exithandler(int sigquit)
{
	printf("\n%d\n", numAlarms);
	exit(1);
}

void handler(int signum) //signal handler
{
	numAlarms++;
	alarm(1);
	signal(SIGINT, exithandler);
}


int main(int argc, char * argv[])
{
	signal(SIGALRM,handler); //register handler to handle SIGALRM
	alarm(1);
	while(1); //busy wait for signal to be delivered
	return 0; //never reached
} 