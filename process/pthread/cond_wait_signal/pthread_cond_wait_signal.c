#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t order_mutex     = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t condition_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition_cond  = PTHREAD_COND_INITIALIZER;

void *functionorder1();
void *functionorder2();
int  order = 1;
#define order_DONE  10
#define order_HALT1  3
#define order_HALT2  6

main()
{
	pthread_t thread1, thread2;

	pthread_create( &thread1, NULL, &functionorder1, NULL);
	pthread_create( &thread2, NULL, &functionorder2, NULL);
	pthread_join( thread1, NULL);
	pthread_join( thread2, NULL);

	exit(0);
}

void *functionorder1()
{
	int sleep_time;
	for(;;)
	{
		printf("functionorder1 1\n");
		pthread_mutex_lock( &condition_mutex );
		if( order != 1 ){
			printf("functionorder1 2\n");
			pthread_cond_wait( &condition_cond, &condition_mutex );
		}
		pthread_mutex_unlock( &condition_mutex );
		printf("functionorder1 3\n");
		
		
		sleep_time = rand() % 10;
		printf("functionorder1 sleep %d\n",sleep_time);
		sleep(sleep_time);
		

		pthread_mutex_lock( &order_mutex );
		order = 2;
		pthread_mutex_unlock( &order_mutex );
		
		printf("functionorder1 4\n");
		pthread_mutex_lock( &condition_mutex );
		pthread_cond_signal(&condition_cond);	
		pthread_mutex_unlock( &condition_mutex );
		
		printf("functionorder1 5\n");
	}
}

void *functionorder2()
{
	int sleep_time;

	for(;;)
	{
		printf("functionorder2 1\n");
		pthread_mutex_lock( &condition_mutex );
		if( order != 2){
			printf("functionorder2 2\n");
			pthread_cond_wait( &condition_cond, &condition_mutex );
		}
		pthread_mutex_unlock( &condition_mutex );
		printf("functionorder2 3\n");
		
		sleep_time = 3;
		printf("functionorder2 sleep %d\n",sleep_time);
		sleep(sleep_time);

		pthread_mutex_lock( &order_mutex );
		order = 1;
		pthread_mutex_unlock( &order_mutex );
		
		printf("functionorder2 4\n");
		pthread_mutex_lock( &condition_mutex );
		pthread_cond_signal(&condition_cond);	
		pthread_mutex_unlock( &condition_mutex );
		
		printf("functionorder2 5\n");
	}

}

