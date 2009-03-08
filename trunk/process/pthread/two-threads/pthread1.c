#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *print_message_function( void *ptr );

main()
{
     pthread_t thread1, thread2;
     char *message1 = "Thread 1";
     char *message2 = "Thread 2";
     int  iret1, iret2;

     /* Create independent threads each of which will execute function */
     // pthread pthread_create( &a_thread, a_thread_attribute, (void *)&thread_function,(void *) &some_argument);
     // 設定這一個thread一開始就執行哪一個function,第四個參數是表示要餵給這一個function什麼數值
     // 並且設定了當這一個thread完成之後，要把回傳值設定給哪一個變數
     iret1 = pthread_create( &thread1, NULL, print_message_function, (void*) message1);
     iret2 = pthread_create( &thread2, NULL, print_message_function, (void*) message2);

     /* Wait till threads are complete before main continues. Unless we  */
     /* wait we run the risk of executing an exit which will terminate   */
     /* the process and all threads before the threads have completed.   */

     pthread_join( thread1, NULL);	// the main process will wait for the end of the thread1
     pthread_join( thread2, NULL); 	// the main process will wait for the end of the thread2
     // 若照這樣說,以上兩個thread必定會依照以上的順序完成

     printf("Thread 1 returns: %d\n",iret1);
     printf("Thread 2 returns: %d\n",iret2);
     exit(0);
}

void *print_message_function( void *ptr )
{
     char *message;
     message = (char *) ptr;
     printf("%s \n", message);
     pthread_exit(0);	// 結束thread
}
