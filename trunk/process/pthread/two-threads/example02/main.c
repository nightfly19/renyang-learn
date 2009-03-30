//==========================include header============================
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
//====================================================================

//==========================define variable===========================
#define NLOOP 65535	// 設定每一個迴圈的個數
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
	pthread_join(tidA,NULL);	// 當執行完這一行指令之後,main的這一個thread會等待tidA這一個thread執行完
	pthread_join(tidB,NULL);	// 當執行完這一行指令之後,main的這一個thread會等待tidB這一個thread執行完

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
