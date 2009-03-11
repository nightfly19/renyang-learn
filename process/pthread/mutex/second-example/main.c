#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
int a=1;
int b=1;
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
#define NTIMES 20

void* do_p1(void* z)
{
  int i;
  for(i=0; i<NTIMES; i++) 
  {
    pthread_mutex_lock(&mutex);
    a += 1;
    b += 1;
    printf("P1: a=%d, b=%d\n", a, b);
    fflush(stdout);	// 把stdout的內容清空，並且輸出
    pthread_mutex_unlock(&mutex);
    sleep(random() % 2);
  }
  return 0;
}

void* do_p2(void* z)
{
  int i;
  for(i=0; i<NTIMES; i++) 
  {
    pthread_mutex_lock(&mutex);
    a *= 2;
    b *= 2;
    printf("P2: a=%d, b=%d\n", a, b);
    fflush(stdout);
    pthread_mutex_unlock(&mutex);
    sleep(random() % 2);
  }
  return 0;
}

int main()
{
  pthread_t thread1, thread2;
  int seed;

  printf("seed: ");
  scanf("%d", &seed);
  srandom(seed);

  pthread_create(&thread1, 0, &do_p1, 0);
  pthread_create(&thread2, 0, &do_p2, 0);
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
}
