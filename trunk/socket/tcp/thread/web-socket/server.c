//====================include header=========================
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
//===========================================================

//====================define variable========================
#define MAXFILES 20
#define SERV "80"
#define MAXLINE 4096
//===========================================================

//====================define structure=======================
struct file {
	char *f_name;	// file name
	char *f_host;	// hostname or IP address
	int f_fd;	// descriptor
	int f_flags;	// F_xxx below
	pthread_t f_tid;
} file[MAXFILES];
//===========================================================

//====================define variable========================
#define F_CONNECTING 1	// connect() in progress
#define F_READING 2	// connect() complete; now reading
#define F_DONE 4	// all done
#define GET_CMD "GET %s HTTP/1.0\r\n\r\n"
//===========================================================

//====================global variable========================
int nconn,nfiles,nlefttoconn,nlefttoread;
//===========================================================

//====================function define========================
void *do_get_read(void *);
void home_page(const char *,const char *);
void write_get_cmd(struct file *);
int tcp_connect(const char *host,const char *serv);
void write_get_cmd(struct file *fptr);
//===========================================================

int main(int argc,char **argv)
{
	int i,n,maxnconn;
	pthread_t tid;
	struct file *fptr;

	if (argc < 5) {
		fprintf(stderr,"usage: web <#conns> <IPaddr> <homepage> file1 ...\n");
		exit(-1);
	}
	maxnconn = atoi(argv[1]);
	nfiles = (argc - 4 < MAXFILES)?argc-4:MAXFILES;

	for (i=0;i<nfiles;i++) {
		file[i].f_name = argv[i+4];
		file[i].f_host = argv[2];
		file[i].f_flags = 0;
	}

	printf("nfiles = %d\n",nfiles);

	home_page(argv[2],argv[3]);

	nlefttoread = nlefttoconn = nfiles;
	nconn = 0;

	// show the main processing loop of the main thread
	while (nlefttoread > 0) {
		while (nconn < maxnconn && nlefttoconn > 0) {
			// find a file to read
			for (i=0;i<nfiles;i++) {
				if (file[i].f_flags ==0)
					break;
			}
			if (i==nfiles) {
				fprintf(stderr,"nlefttoconn = %d but nothing found",nlefttoconn);
				exit(-1);
			}
			file[i].f_flags = F_CONNECTING;
			pthread_create(&tid,NULL,&do_get_read,&file[i]);
			file[i].f_tid = tid;
			nconn++;
			nlefttoconn--;
		}
		if ((n=pthread_join(tid,(void **) &fptr))!=0) {	// there is some problem??
			errno = n;
			fprintf(stderr,"thr_join error\n");
			exit(-1);
		}
		nconn--;
		nlefttoread--;
		printf("thread id %d for %s done\n",tid,fptr->f_name);
	}
	exit(0);

}

//====================function implement=====================
void home_page(const char *host,const char *fname)
{
	int fd,n;
	char line[MAXLINE];

	fd = tcp_connect(host,SERV);	// blocking connect()

	n = snprintf(line,sizeof(line),GET_CMD,fname);
	write(fd,line,n);

	for (;;) {
		if ((n=read(fd,line,MAXLINE))==0)
			break;	// server closed connection

		printf("read %d bytes of home page\n",n);
	}

	printf("end-of-line on home page\n");
	close(fd);
}

int tcp_connect(const char *host,const char *serv)
{
	int sockfd,n;
	struct addrinfo hints,*res,*ressave;
	bzero(&hints,sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((n=getaddrinfo(host,serv,&hints,&res))!=0){
		fprintf(stderr,"tcp_connect error for %s,%s:%s",host,serv,gai_strerror(n));
		exit(-1);
	}
	ressave = res;
	do {
		sockfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
		if (sockfd < 0)
			continue;	// ignore this one
		if (connect(sockfd,res->ai_addr,res->ai_addrlen)==0)
			break;		// success
		close(sockfd);		// ignore this one
	} while ((res=res->ai_next)!=NULL);

	if (res ==NULL) {
		fprintf(stderr,"tcp_connect error for %s,%s",host,serv);
		exit(-1);
	}
	freeaddrinfo(ressave);
	return (sockfd);
}

void *do_get_read(void *vptr)
{
	int fd,n;
	char line[MAXLINE];
	struct file *fptr;
	fptr = (struct file *) vptr;
	fd = tcp_connect(fptr->f_host,SERV);
	fptr->f_fd = fd;
	printf("do_get_read for %s, fd %d, thread %d\n",fptr->f_name,fd,fptr->f_tid);

	write_get_cmd(fptr);	// write() the GET command

	// read server's reply
	for (;;) {
		if ((n=read(fd,line,MAXLINE))==0)
			break;	// server closed connection
		printf("read %d bytes for %s\n",n,fptr->f_name);
	}

	printf("end-of-file on %s\n",fptr->f_name);
	close(fd);
	fptr->f_flags = F_DONE;

	return (fptr);
}

void write_get_cmd(struct file *fptr)
{
	int n;
	char line[MAXLINE];
	n = snprintf(line,sizeof(line),GET_CMD,fptr->f_name);
	write(fptr->f_fd,line,n);
	printf("wrote %d bytes for %s\n",n,fptr->f_name);
	fptr->f_flags = F_READING;
}
//===========================================================
