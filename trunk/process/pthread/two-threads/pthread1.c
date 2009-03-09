#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *print_message_function( void *ptr );

main()
{
     pthread_t thread1, thread2;	// 用來記錄thread的number
     char *message1 = "Thread 1";	// 要給thread的參數
     char *message2 = "Thread 2";
     int  iret1, iret2;

     /* Create independent threads each of which will execute function */
     // pthread pthread_create( &a_thread, a_thread_attribute, (void *)&thread_function,(void *) &some_argument);
     // 設定這一個thread一開始就執行哪一個function,第四個參數是表示要餵給這一個function什麼數值
     iret1 = pthread_create( &thread1, NULL, print_message_function, (void*) message1);
     iret2 = pthread_create( &thread2, NULL, print_message_function, (void*) message2);

     /* Wait till threads are complete before main continues. Unless we  */
     /* wait we run the risk of executing an exit which will terminate   */
     /* the process and all threads before the threads have completed.   */

     printf("before\n");
     pthread_join( thread1, NULL);	// the main process will wait for the end of the thread1
     printf("middle\n");
     // 在middle列印出來之後,絕對不會出現thread1，因為，是執行pthread_join()的結果
     // 會等待指定的thread結束之後,目前這一個main thread才會繼續被執行，
     // 在執行這一個指令之前，是main,thread1,thread2三個同時在進行
     // 執行完這一個指令之後，只剩下thread1,thread2這兩個thread在進行
     pthread_join( thread2, NULL); 	// the main process will wait for the end of the thread2
     printf("after\n");
     // 若照這樣說,以上兩個thread必定會依照以上的順序完成

     printf("Thread 1 returns: %d\n",iret1);
     printf("Thread 2 returns: %d\n",iret2);
     exit(0);
}

void *print_message_function( void *ptr )
{
     char *message;
     message = (char *) ptr;
     int i=0;
     for (i=0;i<66535;i++)
     	printf("%s \n", message);
     pthread_exit(0);	// 結束thread
}
