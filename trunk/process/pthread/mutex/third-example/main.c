//=======================include header========================
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
//=============================================================

//=======================define variable=======================
#define NLOOP 65535
//=============================================================

//=======================global variable=======================
long int counter;
pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;	// incremented by threads
//=============================================================

//=======================define function=======================
void *doit(void *);
//=============================================================

//=======================main function=========================
int main(int argc,char **argv)
{
	pthread_t tidA,tidB;
	int ret_value=0;
	
	printf("before first pthread_create\n");
	ret_value = pthread_create(&tidA,NULL,doit,NULL);
	if (ret_value != 0) {
		fprintf(stderr,"1. pthread_create error\n");
		exit(-1);
	}
	printf("between first and second thread_create\n");
	ret_value = pthread_create(&tidB,NULL,doit,NULL);
	if (ret_value != 0) {
		fprintf(stderr,"2. pthread_create error\n");
		exit(-1);
	}
	printf("after second thread_create\n");
	
	// wait for both threads to terminate
	printf("before first pthread_join\n");
	pthread_join(tidA,NULL);
	printf("between first and second pthread_join\n");
	pthread_join(tidB,NULL);
	printf("after second pthread_join\n");

	exit(0);
}
//=============================================================

//=======================function implement====================
void *doit(void *vptr)
{
	long int i,val;
	// Each thread fetches, prints, and increments the counter NLOOP times.
	// The value of the counter should increase monotonically

	for (i=0;i<NLOOP;i++) {
		pthread_mutex_lock(&counter_mutex);
		val = counter;
		printf("%u: %ld\n",pthread_self(),val+1);
		counter=val+1;
		pthread_mutex_unlock(&counter_mutex);
	}
	
	return (NULL);
}
