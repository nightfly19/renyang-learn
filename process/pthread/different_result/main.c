#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
void thread(void)
{
	int i;
	for(i=0;i<65535;i++)
		printf("This is a pthread.\n");
}

int main(void)
{
	pthread_t id;
	int i,ret;
	ret=pthread_create(&id,NULL,(void *) thread,NULL);
	if(ret!=0){
		printf ("Create pthread error!\n");
		exit (1);
	}
	for(i=0;i<65535;i++)
		printf("This is the main process.\n");
	pthread_join(id,NULL);	// 等待id這一個thread結束才執行下一步,我覺得這一個用法很好,在結束前放一個
	return (0);
}
