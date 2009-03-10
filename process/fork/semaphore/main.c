#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
sem_t sem;
main()
{
  int rc; 
  rc = sem_init(&sem, 1, 1);
  if (rc == -1) {
    perror("sem_init() failed");
    exit(1);
  }
  int pid = fork();
  if (pid==0) /* child */
  {
    int i;
    srand(1234);
    for(i=0; i<30; i++)
     {
      sem_wait(&sem);
      sleep(rand()&3);
      printf("child: %d\n", i);
      sem_post(&sem);
     }
  }
  else /* parent */
  {
    int i;
    srand(54321);
    for(i=0; i<30; i++)
     {
      sem_wait(&sem);
      sleep(rand()&3);
      printf("parent: %d\n", i);
      sem_post(&sem);
     }
  }
}
