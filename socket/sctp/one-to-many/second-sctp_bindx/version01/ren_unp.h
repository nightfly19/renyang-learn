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
//================================================================


#endif
