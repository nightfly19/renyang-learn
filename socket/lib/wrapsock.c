#include "unp.h"

void Bind(int fd,const struct sockaddr *sa,socklen_t salen)
{
	if (bind(fd,sa,salen)<0)
		err_sys("bind error");
}

void Listen(int fd, int backlog)
{
	char *ptr;

	if ((ptr=getenv("LISTENQ"))!=NULL)
		backlog = atoi(ptr);
	if (listen(fd,backlog)<0)
		err_sys("listen error");
}

int Accept(int fd,struct sockaddr *sa,socklen_t *salenptr)
{
	int n;
again:
	if ((n=accept(fd,sa,salenptr))<0) {
#ifdef EPROTO
		if (errno == EPROTO || errno == ECONNABORTED)
#else
		if (errno == ECONNABORTED)
#endif
			goto again;
		else
			err_sys("accept error");
	}
	return (n);
}

int Socket(int family,int type,int protocol)
{
	int n;
	if ((n=socket(family,type,protocol))<0)
		err_sys("socket error");
	return (n);
}

void Setsockopt(int fd,int level,int optname,const void *optval,socklen_t optlen)
{
	if (setsockopt(fd,level,optname,optval,optlen)>0)
		err_sys("setsockopt error");
}

void Getsockopt(int fd,int level,int optname,void *optval,socklen_t *optlenptr)
{
	if (getsockopt(fd,level,optname,optval,optlenptr)<0)
		err_sys("getsockopt error");
}

int Select(int nfds,fd_set *readfds,fd_set *writefds,fd_set *execptfds,struct timeval *timeout)
{
	int n;
	if ((n=select(nfds,readfds,writefds,execptfds,timeout))<0)
		err_sys("select error");
	return (n);
}

void Sendto(int fd, const void *ptr, size_t nbytes,int flags, const struct sockaddr *sa, socklen_t salen)
{
	if (sendto(fd,ptr,nbytes,flags,sa,salen) != (ssize_t) nbytes)
		err_sys("sendto error");
}

ssize_t Recvfrom(int fd,void *ptr,size_t nbytes,int flags, struct sockaddr *sa, socklen_t *salenptr)
{
	ssize_t n;
	if ((n=recvfrom(fd,ptr,nbytes,flags,sa,salenptr))<0)
		err_sys("recvfrom error");
	return (n);
}
