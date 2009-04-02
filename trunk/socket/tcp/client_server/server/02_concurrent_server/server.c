//=============================include header===========================
#include "unp.h"
//======================================================================

//=============================main function============================
int main(int argc,char **argv)
{
	int listenfd,connfd;
	pid_t childpid;
	socklen_t clilen,addrlen;
	struct sockaddr *cliaddr;

	if (argc == 2)
		listenfd = Tcp_listen(NULL,argv[1],&addrlen);
	else if (argc == 3)
		listenfd = Tcp_listen(argv[1],argv[2],&addrlen);
	else
		err_quit("usage: server [<host>] <port#>");

	cliaddr = Malloc(addrlen);

	Signal(SIGCHLD,sig_chld);
	Signal(SIGINT,sig_int);

	for (;;) {
		clilen = addrlen;
		if ((connfd = accept(listenfd,cliaddr,&clilen))<0) {
			if (errno == EINTR)
				continue;
			else
				err_sys("accept error");
		}
		if ((childpid == Fork()) == 0) {
			Close(listenfd);
			web_child(connfd);
			exit(0);
		}
		Close(connfd);
	}
}
//======================================================================

//=============================function implement=======================
//======================================================================
