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
struct file {		// 宣告一個結構陣列
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
	// 設定連線個數,把字串轉成數字
	maxnconn = atoi(argv[1]);
	// 設定要處理的檔案個數,參數個數減掉前四個必要參數,剩下的都是file名稱
	nfiles = (argc - 4 < MAXFILES)?argc-4:MAXFILES;

	for (i=0;i<nfiles;i++) {
		file[i].f_name = argv[i+4];	// 檔案名稱
		file[i].f_host = argv[2];	// 設定server ip
		file[i].f_flags = 0;		// 為0,表示這一個檔案還沒有被處理
	}

	// 列印出來要處理的檔案個數
	printf("nfiles = %d\n",nfiles);

	home_page(argv[2],argv[3]);

	// 剩下要去[讀取的],[要準備連接的],[一共要讀取的檔案]
	nlefttoread = nlefttoconn = nfiles;
	// 目前連接數
	nconn = 0;

	// show the main processing loop of the main thread
	while (nlefttoread > 0) {
		while (nconn < maxnconn && nlefttoconn > 0) {
			// find a file to read
			for (i=0;i<nfiles;i++) {
				if (file[i].f_flags ==0)
					break;
			}
			if (i==nfiles) {	// 若這一個條件成立,表示所有的檔案都被處理完,或正在處理中
				fprintf(stderr,"nlefttoconn = %d but nothing found",nlefttoconn);
				exit(-1);	// 直接中斷程式這樣對嗎?
			}
			file[i].f_flags = F_CONNECTING;	// 正在連線中
			pthread_create(&tid,NULL,do_get_read,&file[i]);	// 產生一個thread,並執行do_get_read()
			file[i].f_tid = tid;		// 設定處理這一個file的thread id是哪一個
			nconn++;			// 增加一個連線數
			nlefttoconn--;			// 減少剩下要被連線的個數
		}
		if ((n=pthread_join(tid,(void **) &fptr))!=0) {	// there is some problem??
			errno = n;
			fprintf(stderr,"thr_join error\n");
			exit(-1);
		}
		nconn--;	// 應該是處理完一個,所以,連線數減一
		nlefttoread--;	// 減少還要處理的檔案個數
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

	// 列印要執行的指令
	n = snprintf(line,sizeof(line),GET_CMD,fname);
	// 送出指令
	write(fd,line,n);

	for (;;) {
		if ((n=read(fd,line,MAXLINE))==0)	// 表示對方結束connection
			break;	// server closed connection

		// 列印出讀取多少個bytes
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
	// 設定依條件所建立的socket
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((n=getaddrinfo(host,serv,&hints,&res))!=0){
		fprintf(stderr,"tcp_connect error for %s,%s:%s",host,serv,gai_strerror(n));
		exit(-1);
	}
	// res內儲存許多要建立的socket參數
	ressave = res;
	do {
		// 嘗試建立socket
		sockfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
		if (sockfd < 0)		// 建立失敗,尋找下一個要建立socket的參數
			continue;	// ignore this one
		if (connect(sockfd,res->ai_addr,res->ai_addrlen)==0)
			break;		// success
		close(sockfd);		// ignore this one
	} while ((res=res->ai_next)!=NULL);

	if (res ==NULL) {	// 利用了所以可以建立socket的參數,但均是失敗,則列印出錯誤
		fprintf(stderr,"tcp_connect error for %s,%s",host,serv);
		exit(-1);
	}
	// 把剛剛收集到的可以嘗試建立socket的參數陣列的空間釋放出來
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
