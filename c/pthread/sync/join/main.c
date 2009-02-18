#include <stdio.h>
#include <pthread.h>

#define NTHREADS 10
void *thread_function(void *);
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;	// 初始值
int  counter = 0;

main()
{
   pthread_t thread_id[NTHREADS];	// 宣告十個pthread,pthread is unsigned long integer
   int i, j;

   for(i=0; i < NTHREADS; i++)
   {
      pthread_create( &thread_id[i], NULL, thread_function, NULL );	// 建立十個thread
   }

   for(j=0; j < NTHREADS; j++)
   {
      pthread_join( thread_id[j], NULL); 	// 設定main必需等待thread_id[j]執行結束
   }
  
   /* Now that all threads are complete I can print the final result.     */
   /* Without the join I could be printing a value before all the threads */
   /* have been completed.                                                */

   printf("Final counter value: %d\n", counter);
}

void *thread_function(void *dummyPtr)
{
   printf("Thread number %llu\n", pthread_self());	// 取得thread自己的thread id
   pthread_mutex_lock( &mutex1 );	// lock住這一個thread
   counter++;
   pthread_mutex_unlock( &mutex1 );	// 釋放mutex
   pthread_exit(0);
}
