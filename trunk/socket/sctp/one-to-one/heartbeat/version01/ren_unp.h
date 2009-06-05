#ifndef REN_UNP_H
#define REN_UNP_H

//=========================include header=========================
#include <sys/socket.h>		// for socket
#include <netinet/in.h>		// for socket
#include <netinet/sctp.h>	// for sctp
#include <stdarg.h>		// for variable-length argument
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//================================================================

//=========================define variable========================
#define BUFFERSIZE 8192
#define SERV_PORT 9877
#define LISTENQ 1024
//================================================================

//=========================define function========================
int Socket(int family,int type,int protocol)
{
	int n;
	if ((n=socket(family,type,protocol))<0) {
		fprintf(stderr,"socket error");
		exit(-1);
	}
	return n;
}

int heartbeat_action(int sock_fd,struct sockaddr *sa,socklen_t salen,u_int value)
{
        struct sctp_paddrparams sp; 
        int siz;
	int ret_value;

        bzero(&sp,sizeof(sp));
        memcpy(&sp.spp_address,sa,salen);
	// 每10ms送一次heartbeat
        sp.spp_hbinterval = value; 
	// 設定重送次數
        sp.spp_pathmaxrxt=0;
	// 設定Maximum Transmission Unit
        sp.spp_pathmtu = 1500;
	// 設定啟動heartbeat
        sp.spp_flags = SPP_HB_ENABLE;
        ret_value = setsockopt(sock_fd,IPPROTO_SCTP,SCTP_PEER_ADDR_PARAMS,&sp,sizeof(sp));
        if (ret_value ==-1)
        {
                perror("set heart beat error");
        }
}
//================================================================


#endif
