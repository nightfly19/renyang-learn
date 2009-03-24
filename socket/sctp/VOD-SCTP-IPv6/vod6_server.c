/* Written by J. S. Ha (mugal1@cs.knu.ac.kr), October 2005 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <pthread.h>
#include <netinet/sctp.h>

#define BUFSIZE  1024*4

void * clnt_connection(void *arg);
void send_message(char * message, int len);
void error_handling(char *message);

int clnt_number=0;
int clnt_socks[10];
pthread_mutex_t mutx;

char vod_list[100]=("##play-list####1.채연-둘이서 \n##2.코요테-빙고 \n##3.유니-call3\n");

int main(int argc, char **argv)
{
  int serv_sock;
  int clnt_sock;
  struct sockaddr_in6 serv_addr;
  struct sockaddr_in6 clnt_addr;
  int clnt_addr_size;
  pthread_t thread;
  
  if(argc!=2) {
    printf("Usage : %s <port>\n", argv[0]);
    exit(1);
  }
  
  if(pthread_mutex_init(&mutx, NULL))
    error_handling("mutex init error");
  ////////////////////////////////////////////////////
//  serv_sock=socket(PF_INET, SOCK_STREAM, IPPROTO_SCTP);

//  memset(&serv_addr, 0, sizeof(serv_addr));
//  serv_addr.sin_family=AF_INET; 
//  serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
//  serv_addr.sin_port=htons(atoi(argv[1]));
  /////////////////////////////////////////////////////
  serv_sock = socket(AF_INET6, SOCK_STREAM,IPPROTO_SCTP);
  if (serv_sock < 0)
  {
        perror("socket create error:");
        exit(0);
  }

    // bind 를 위해서 소켓특성을 묶어준다. 
    // IPv6 버젼으로 맞춘다.
    serv_addr.sin6_family   = AF_INET6;
    serv_addr.sin6_flowinfo = 0;
    serv_addr.sin6_port     = htons(atoi(argv[1]));
    serv_addr.sin6_addr=in6addr_any;
    // in6addr_any 는 *: 를 나타낸다.
    // ipv4 에서의 htonl(INADDR_ANY) 와 동일하다고 볼수 있다. 
   //inet_pton(AF_INET6,"2001::153",(void *)&ipv6_addr);
   //	memcpy((void *)&sin6.sin6_addr,(void *)&ipv6_addr,16);

  if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1)
    error_handling("bind() error");
  
  if(listen(serv_sock, 5)==-1)
    error_handling("listen() error");

  printf("서버가 실행되었습니다.\n"); 

  while(1){
    clnt_addr_size=sizeof(clnt_addr);
    clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr,&clnt_addr_size);

	pthread_mutex_lock(&mutx);
    clnt_socks[clnt_number++]=clnt_sock;
    pthread_mutex_unlock(&mutx);

    pthread_create(&thread, NULL, clnt_connection, (void*)clnt_sock);   
 //   printf("새로운 연결, 클라이언트 IP : %s \n", inet_ntoa(clnt_addr.sin6_addr));
	 printf("새로운 연결이 발생했습니다\n"); 
  }
 
  return 0;
}

void * clnt_connection(void *arg)
{
  int clnt_sock= (int)arg;
  int str_len=0;
  char message[BUFSIZE];
  int i;

  int fdin;
  int flags;
  int n;
  char inbuf[BUFSIZE];
  char file_name[100];
  
  char vod1[20]="vod1.avi";
  char vod2[20]="vod2.avi";
  char vod3[20]="vod3.avi";

	
  flags=O_RDONLY;

  write(clnt_sock,vod_list,sizeof(vod_list));
  while( (str_len=read(clnt_sock, message, sizeof(message)-1)) != 0)
  {
	  if(!strncmp(message,"##1",3)||!strncmp(message,"##2",3)||!strncmp(message,"##3",3)) 
	  {
		  //printf("%s파일을 전송합니다\n",message);

		  if(!strncmp(message,"##1",3))
		  {
			  sprintf(file_name, "%s", vod1);
			  printf("##1.채연-둘이서 서비스를 요청 받았습니다. 전송을 실행합니다.\n");
		  }
		  else if(!strncmp(message,"##2",3))
		  {
              sprintf(file_name, "%s", vod2);
			  printf("##2.코요테-빙고  서비스를 요청 받았습니다. 전송을 실행합니다.\n");
		  }
		  else if(!strncmp(message,"##3",3))
		  {
			  sprintf(file_name, "%s", vod3);
			  printf("##3.유니-call3 서비스를 요청 받았습니다. 전송을 실행합니다.\n");
		  }
		  
		  write(clnt_sock,"***start",8);//파일 전송 알림
		  if((fdin=open(file_name,flags))==-1)
		  {
				perror("file open error\n");
				exit(1);
		  }
		  while((n=read(fdin,inbuf,BUFSIZE))>0)
		  {
				//	printf("%s",inbuf);
					write(clnt_sock,inbuf,BUFSIZE);
				//	printf("%s              .....  \n",inbuf);
					
		  }
		  
		  write(clnt_sock,"endss",3);
		  printf("서비스 전송을 완료 하였습니다.\n");
		  close(fdin);

	   }

  }
  pthread_mutex_lock(&mutx);
  for(i=0; i<clnt_number; i++){   /* 클라이언트 연결 종료 시 */
    if(clnt_sock == clnt_socks[i]){
      for( ; i<clnt_number-1; i++)
		  clnt_socks[i]=clnt_socks[i+1];
	 
      break;
    }
  }
  clnt_number--;
  pthread_mutex_unlock(&mutx);

  printf("클라이언트의 연결이 종료 되엇습니다\n");
  close(clnt_sock);
  return 0;
}

void send_message(char * message, int len)
{
  int i;
  pthread_mutex_lock(&mutx);
  for(i=0; i<clnt_number; i++)
    write(clnt_socks[i], message, len);
  pthread_mutex_unlock(&mutx);
}

void error_handling(char *message)
{
  fputs(message, stderr);
  fputc('\n', stderr);
  exit(1);
}
