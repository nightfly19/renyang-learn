#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t count_mutex     = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t condition_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition_cond  = PTHREAD_COND_INITIALIZER;	// 靜態建立鎖

void *functionCount1();
void *functionCount2();
int  count = 0;
#define COUNT_DONE  10
#define COUNT_HALT1  3
#define COUNT_HALT2  6

main()
{
   pthread_t thread1, thread2;

   // printf("1\n");
   pthread_create( &thread1, NULL, &functionCount1, NULL);	// 建立一個thread，要執行函數的起點是functionCount1
   //printf("2\n");
   pthread_create( &thread2, NULL, &functionCount2, NULL);	// 建立一個thread,要執行函數的起點是functionCOunt2
   // printf("3\n");
   pthread_join( thread1, NULL);	// 暫停main，直到這一個thread1結束
   pthread_join( thread2, NULL);	// 暫停main，直到這一個thread2結束

   exit(0);
}

void *functionCount1()
{
   for(;;)
   {
      // printf("Function1\n");	// test
      pthread_mutex_lock( &condition_mutex );	// 鎖住condition_mutex
      while( count >= COUNT_HALT1 && count <= COUNT_HALT2 )
      {
	 // printf("function1 before pthread_cond_wait\n");	// test
         pthread_cond_wait( &condition_cond, &condition_mutex );	// 使thread睡覺，等待別的程序來叫醒它,同時釋放condition_mutex
	 // printf("function1 after pthread_cond_wait\n");	// test
      }
      pthread_mutex_unlock( &condition_mutex );

      pthread_mutex_lock( &count_mutex );
      count++;
      printf("Counter value functionCount1: %d\n",count);
      pthread_mutex_unlock( &count_mutex );

      if(count >= COUNT_DONE) return(NULL);
    }
}

void *functionCount2()
{
    for(;;)
    {
       // printf("function2\n");	// test
       pthread_mutex_lock( &condition_mutex );
       if( count < COUNT_HALT1 || count > COUNT_HALT2 )
       {
	  // printf("function2 before pthread_cond_wait\n");	// test
          pthread_cond_signal( &condition_cond );	// 叫醒因condition_cond在等待的thread
	  // printf("function2 after pthread_cond_wait\n");	// test
       }
       pthread_mutex_unlock( &condition_mutex );
       // printf("function2 after pthread_mutex_unlock\n");	// test
       pthread_mutex_lock( &count_mutex );
       count++;
       printf("Counter value functionCount2: %d\n",count);
       pthread_mutex_unlock( &count_mutex );

       if(count >= COUNT_DONE) return(NULL);
    }
}
