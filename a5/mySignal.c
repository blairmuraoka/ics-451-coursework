/**
 *	Author: Blair Muraoka
 *	ICS 451 - Ravi Narayan
 */

/* mySignal.c */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

static int isCalled = 0;

void handler(int signum) //signal handler
{
	isCalled = 1;
	printf("Hello World!\n");
	alarm(1);
}

int main(int argc, char * argv[])
{
	signal(SIGALRM,handler); //register handler to handle SIGALRM
	alarm(1); //Schedule a SIGALRM for 1 second	
	while(1) {
		if (isCalled == 1) {
			printf("Turing was right!\n");
			isCalled = 0;
		}
	}
	//busy wait for signal to be delivered
	return 0; //never reached
} 