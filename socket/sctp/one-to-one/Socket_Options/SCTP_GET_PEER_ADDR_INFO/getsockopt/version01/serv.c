#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/sctp.h>
#include "term-project/common.h"

int main()
{
  // descriptor of server and client socket
  int listenSock,connSock;
  int ret;
  char buffer[MAX_BUFFER];
  struct sockaddr_in serv_addr,cli_addr;
  int server_len,client_len;
  int portno = MY_PORT_NUM ;

  // This cmsghdr structure provides information for initializing new SCTP associations with sendmsg().
  // The SCTP_INITMSG socket option uses this same data structure.
  // This structure is not used for recvmsg().
  // struct sctp_initmsg {
  //     __u16 sinit_num_ostreams;
  //     __u16 sinit_max_instreams;
  //     __u16 sinit_max_attempts;
  //     __u16 sinit_max_init_timeo;
  // };
  // 用來設定一些sctp的設定值
  struct sctp_initmsg initmsg;

  // listenSock=socket(AF_INET, SOCK_STREAM, 0);=>表示使用one-to-one的情況
  // socket(AF_INET,SOCK_SEQPACKET,IPPROTO_SCTP);
  // 就是使用sctp的one-to-many
  // 不相容於tcp,我猜的
  listenSock = socket( AF_INET, SOCK_STREAM, IPPROTO_SCTP );

  if (listenSock < 0) {
  	perror("ERROR opening socket");
	exit(1);
  }
  printf("connSock : %d  \n",listenSock);

  //bzero((char *) &serv_addr, sizeof(serv_addr));
  memset(&serv_addr,0,sizeof(struct sockaddr_in));

  //SCTP option
  /* Specify that a maximum of 5 streams will be available per socket */

  // set the init number of the stream
  initmsg.sinit_num_ostreams = 5;
  initmsg.sinit_max_instreams = 5;
  // the max attempts is four times
  initmsg.sinit_max_attempts = 4;
  ret = setsockopt( listenSock, IPPROTO_SCTP, SCTP_INITMSG,
		  &initmsg, sizeof(initmsg) );
  if (ret < 0) {
  	perror("ERROR setsockopt");
	exit(1);
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  // bind - bind a name to a socket
  if ((bind(listenSock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)))==-1) {
  	perror("Bind error");
	exit(1);
  }
  // listen - listen for socket connections and limit the queue of incoming connections
  if (listen( listenSock, 5 )==-1) {
  	perror("listen error");
	exit(1);
  }
  while (1) {
     printf("Accepting connections ...\n");
     client_len = sizeof(cli_addr);
     connSock = accept( listenSock, (struct sockaddr *)&cli_addr, &client_len);
     if (connSock < 0) {
     	perror("accept error");
	exit(1);
     }

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
