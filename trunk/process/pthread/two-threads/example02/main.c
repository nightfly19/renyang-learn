//==========================include header============================
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
//====================================================================

//==========================define variable===========================
#define NLOOP 65535
//====================================================================

//==========================global variable===========================
int counter;	// incremented by threads
//====================================================================

//==========================define function===========================
void *doit(void *);
//====================================================================

//==========================main function=============================
int main(int argc,char **argv)
{
	pthread_t tidA,tidB;
	if (pthread_create(&tidA,NULL,&doit,NULL)!=0) {
		fprintf(stderr,"pthread_create A error\n");
		exit(-1);
	}
	if (pthread_create(&tidB,NULL,&doit,NULL)!=0) {
		fprintf(stderr,"pthread_create B error\n");
		exit(-1);
	}
	// wait for both threads to terminate
	pthread_join(tidA,NULL);
	pthread_join(tidB,NULL);

	return 0;
}
//====================================================================

//==========================function implement========================
void *doit(void *vptr)
{
	int i,val;
	// Each thread fetches, prints, and increments the counter NLOOP times.
	// The value of the counter should increase monotonically

	for (i=0;i<NLOOP;i++) {
		val = counter;
		printf("%u: %d\n",pthread_self(),val+1);
		counter = val + 1;
	}
	return (NULL);
}
