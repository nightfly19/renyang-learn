
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>

#include "term-project/common.h"

int main()
{
  int connSock, in, i, ret, flags;
  struct sockaddr_in servaddr,cliaddr;
  struct sctp_status status;
  struct sctp_sndrcvinfo sndrcvinfo;
  struct sctp_event_subscribe events;
  struct sctp_initmsg initmsg;
  // 新增 - 查看對方的網路情況
  struct sctp_paddrinfo paddrinfo;
  socklen_t paddrinfolen;
  socklen_t cliaddrlen;
  // 新增 - 結束
  char buffer[MAX_BUFFER+1];
  paddrinfolen = sizeof(paddrinfo);
  cliaddrlen = sizeof(cliaddr);

  /* Create an SCTP TCP-Style Socket */
  connSock = socket( AF_INET, SOCK_STREAM, IPPROTO_SCTP );
  if (connSock < 0) {
  	perror("socket error");
	exit(1);
  }

/*
 * 2.1 socket 
 * int socket(int domain, int type,int protocol)  
 * domain:說明我們網絡程序所在的主機採用的通訊協族(AF_UNIX和AF_INET等).
 * AF_UNIX只能夠用單一的Unix系統進程間通信,
 * 而AF_INET是針對Internet的,因而可以允許在遠程主機之間通信
 * (當我們 man socket時發現 domain可選項是 PF_*而不是AF_*,
 * 因為glibc是posix的實現 所以用PF代替了AF,不過我們都可以使用的). 
 *
 * type:我們網絡程序所採用的通訊協議(SOCK_STREAM,SOCK_DGRAM等)
 * SOCK_STREAM表明我們用的是TCP 協議,這樣會提供按順序的,
 * 可靠,雙向,面向連接的比特流.
 * SOCK_DGRAM 表明我們用的是UDP協議,這樣只會提供定長的,不可靠,無連接的通信. 
 *
 * protocol:由我們指定了type,所以這個地方我們一般只要用0來代替就可以了
 * socket為網絡通訊做基本的準備.成功時返回文件描述符,失敗時返回-1,
 * 看errno可知道出錯的詳細情況.  
 */

  /* Specify that a maximum of 5 streams will be available per socket */
  memset( &initmsg, 0, sizeof(initmsg) );

  //memset - fill memory with a constant byte
  
  initmsg.sinit_num_ostreams = 5;
  initmsg.sinit_max_instreams = 5;
  initmsg.sinit_max_attempts = 4;

  ret = setsockopt( connSock, IPPROTO_SCTP, SCTP_INITMSG,
                     &initmsg, sizeof(initmsg) );
  if (ret < 0) {
  	perror("setsockopt error");
	exit(1);
  }
  /*
   * int setsockopt(
   * SOCKET s,
   * int level,
   * int optname,
   * const char* optval,
   * int optlen
   * );
   *
   * s(套接字): 指向一个打开的套接口描述字
   * level:(级别)： 指定选项代码的类型。
   * SOL_SOCKET: 基本套接口
   * IPPROTO_IP: IPv4套接口
   * IPPROTO_IPV6: IPv6套接口
   * IPPROTO_TCP: TCP套接口
   * optname(选项名)： 选项名称
   * optval(选项值): 是一个指向变量的指针 类型：整形，套接口结构， 其他结构类型:linger{}, timeval{ }
   * optlen(选项长度) ：optval 的大小
   *
   * 返回值：标志打开或关闭某个特征的二进制选项
   */

  /* Specify the peer endpoint to which we'll connect */

  bzero( (void *)&servaddr, sizeof(servaddr) );
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(MY_PORT_NUM);
  servaddr.sin_addr.s_addr = inet_addr( "127.0.0.1" );

  /* Connect to the server */
  ret = connect( connSock, (struct sockaddr *)&servaddr, sizeof(servaddr) );
  if (ret < 0) {
  	perror("connect error");
	exit(1);
  }

  /* Enable receipt of SCTP Snd/Rcv Data via sctp_recvmsg */
  memset( (void *)&events, 0, sizeof(events) );
  events.sctp_data_io_event = 1;
  ret = setsockopt( connSock, IPPROTO_SCTP, SCTP_EVENTS,
                     (const void *)&events, sizeof(events) );
  if (ret < 0) {
  	perror("setsockopt error");
	exit(1);
  }

  /* Read and emit the status of the Socket (optional step) */
  in = sizeof(status);
  ret = getsockopt( connSock, IPPROTO_SCTP, SCTP_STATUS,
                     (void *)&status, (socklen_t *)&in );
  if (ret < 0) {
  	perror("getsockopt error");
	exit(1);
  }

  printf("assoc id  = %d\n", status.sstat_assoc_id );
  printf("state     = %d\n", status.sstat_state );
  printf("rwnd      = %d\n", status.sstat_rwnd);
  printf("unack data= %d\n", status.sstat_unackdata);
  printf("pend data = %d\n", status.sstat_penddata);
  printf("instrms   = %d\n", status.sstat_instrms );
  printf("outstrms  = %d\n", status.sstat_outstrms );
  printf("fragmentation point = %d\n", status.sstat_fragmentation_point);

  /* Expect two messages from the peer */

  for (i = 0 ; i < 2 ; i++) {

    in = sctp_recvmsg( connSock, (void *)buffer, sizeof(buffer),
                        (struct sockaddr *) &cliaddr, &cliaddrlen, &sndrcvinfo, &flags );
    if (in < 0) {
    	perror("sctp_recvmsg error");
	exit(1);
    }
    
    // 要取得對方的網路情況
    bzero(&paddrinfo,sizeof(paddrinfo));
    // 設定要監控的網路環境
    memcpy(&paddrinfo.spinfo_address,&cliaddr,sizeof(cliaddr));
    ret = getsockopt(connSock,IPPROTO_SCTP,SCTP_GET_PEER_ADDR_INFO,&paddrinfo,&paddrinfolen);
    if (ret == -1) {
    	perror("SCTP_GET_PEER_ADDR ERROR");
    }
    else
    {
    	printf("assoc_id:%d\n",paddrinfo.spinfo_assoc_id);
	printf("state:%d\n",paddrinfo.spinfo_state);
	printf("cwnd:%d\n",paddrinfo.spinfo_cwnd);
	printf("srtt:%d\n",paddrinfo.spinfo_srtt);
	printf("rto:%d\n",paddrinfo.spinfo_rto);
	printf("mtu:%d\n",paddrinfo.spinfo_mtu);
    }

    if (in > 0) {
      buffer[in] = 0;
      if (sndrcvinfo.sinfo_stream == DATA_STREAM) {
        printf("(Local) %s\n", buffer);
      }
    }

  }

  /* Close our socket and exit */
  close(connSock);

  return 0;
}

