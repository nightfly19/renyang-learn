#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <netinet/sctp.h>
#include "term-project/common.h"

int main()
{
  int listenSock,connSock;
  int ret;
  char buffer[256];
  struct sockaddr_in serv_addr;
  int portno = MY_PORT_NUM ;

  struct sctp_initmsg initmsg;

  // listenSock=socket(AF_INET, SOCK_STREAM, 0);
  listenSock = socket( AF_INET, SOCK_STREAM, IPPROTO_SCTP );

  if (listenSock < 0)
  	perror("ERROR opening socket");
  printf("connSock : %d  \n",listenSock);

  //bzero((char *) &serv_addr, sizeof(serv_addr));
  memset(&serv_addr,0,sizeof(struct sockaddr_in));

  //SCTP option
  /* Specify that a maximum of 5 streams will be available per socket */

  initmsg.sinit_num_ostreams = 5;
  initmsg.sinit_max_instreams = 5;
  initmsg.sinit_max_attempts = 4;
  ret = setsockopt( listenSock, IPPROTO_SCTP, SCTP_INITMSG,
		  &initmsg, sizeof(initmsg) );


  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  bind(listenSock, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
  listen( listenSock, 5 );
  while (1) {
     printf("Accepting connections ...\n");
     connSock = accept( listenSock, (struct sockaddr *)NULL, (int *)NULL );

     // 送資料
     //
     // int sctp_sendmsg(int sd, const void * msg, size_t len,
     //	     struct sockaddr *to, socklen_t tolen,
     //	     uint32_t ppid, uint32_t flags,
     //	     uint16_t stream_no, uint32_t timetolive,
     //	     uint32_t context);
     //
     snprintf( buffer, MAX_BUFFER, "%s","xxxxxxxxxxXX");
     ret = sctp_sendmsg( connSock, (void *)buffer, (size_t)strlen(buffer),
                          NULL, 0, 0, 0, DATA_STREAM, 0, 0 );

     close( connSock );
  }
  return 0;
}
