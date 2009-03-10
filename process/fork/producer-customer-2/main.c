//=========================include header==================
#include <stdio.h>
#include <sys/types.h> /* standard system data types.  */
#include <sys/ipc.h>   /* IPC structures. */
#include <sys/msg.h>   /* message-queue functions. */
#include <string.h>
#include <stdlib.h>
//=========================================================

//========================global variable==================
int mayproduce;
int mayconsume;
#define MAX_MSG_LEN   30  /* 訊息最大長度 */
#define CAPACITY      3   /* 緩衝區能容納的訊息數 */
struct msgbuf             /* 訊息的資料型態 */  
  {
    long int mtype; /* type of received/sent message */
    char mtext[1];  /* text of the message */
  };
//=========================================================

//========================define function==================
struct msgbuf *null;
void producer();
void consumer();
//=========================================================

//=========================main function=========================
int main() {
  int pid;   int i;
  mayproduce = msgget(IPC_PRIVATE, 0600); /* owner rw- */
  mayconsume = msgget(IPC_PRIVATE, 0600); /* owner rw- */
  if (mayproduce == -1 || mayconsume == -1) {
    perror("msgget");  exit(1);
  }
  null = malloc(sizeof(struct msgbuf));
  null->mtype = 1;  /* 必須 > 0 */
  null->mtext[0] = 0;
  for(i=1; i<=CAPACITY; i++) {
    int rc = msgsnd(mayproduce, null, 1, 0); /*訊息長度=1 */
    if (rc == -1) {
      perror("msgsnd");  exit(1);    
    }
  }
  pid = fork();
  if (pid == 0)
   consumer();
  else
    producer();
  return 0;
}
//========================================================================

//=========================function implementation=========================
void producer() 
{
  int i;
  struct msgbuf *pmsg = malloc(sizeof(struct msgbuf)
                               + MAX_MSG_LEN);
  printf("Producer Start\n");
  for(i=0; i<10; i++)  /* 生產10個產品 */
  {
    int rc=msgrcv(mayproduce, pmsg, MAX_MSG_LEN,0,0);
    if (rc == -1) {
      perror("msgrcv");  exit(1);
    }
    pmsg->mtype = 1;
    sprintf(pmsg->mtext, "Item No. %d", i);
    printf("Produce: %s\n", pmsg->mtext);
    msgsnd(mayconsume,pmsg,strlen(pmsg->mtext)+1, 0);
  }
}

void consumer() 
{
  int rc;
  struct msgbuf *cmsg = malloc(sizeof(struct msgbuf) + MAX_MSG_LEN);
  printf("Consumer Start\n");
  while (1) {
    rc = msgrcv(mayconsume, cmsg, MAX_MSG_LEN, 0, 0);
    if (rc == -1) break;
    printf("Consume: %s\n", cmsg->mtext);
    msgsnd (mayproduce, null, 1, 0);
  }
}
