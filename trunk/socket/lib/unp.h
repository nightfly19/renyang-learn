#ifndef UNP_H
#define UNP_H

//========================include header==========================
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/sctp.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/wait.h>
//================================================================

//========================define variable=========================
#define MAXLINE 4096
#define LISTENQ 1024
#define SA struct sockaddr
#define SCTP_PDAPI_INCR_SZ 65535   /* increment size for pdapi when adding buf space */
#define SCTP_PDAPI_NEED_MORE_THRESHOLD 1024	/* need more space threshold */
#define SERV_MAX_SCTP_STRM	10	/* normal maximum streams */
//================================================================

//========================typedef=================================
typedef void Sigfunc(int);
//================================================================

//========================define function=========================
//------------------------sctp_wrapper.c--------------------------
int Sctp_recvmsg(int,void *,size_t,struct sockaddr *,socklen_t *,struct sctp_sndrcvinfo *,int *);
int Sctp_sendmsg(int,void *,size_t,struct sockaddr *,socklen_t,uint32_t,uint32_t,uint16_t,uint32_t,uint32_t);
int Sctp_bindx(int,struct sockaddr *,int,int);
//----------------------------------------------------------------
//------------------------sctp_strcliecho.c-----------------------
void sctpstr_cli_echoall(FILE *,int,struct sockaddr *,socklen_t);
//----------------------------------------------------------------
//------------------------sctp_strcli.c---------------------------
void sctpstr_cli(FILE *,int,struct sockaddr *,socklen_t);
//----------------------------------------------------------------
//------------------------wrapunix.c------------------------------
void Close(int);
void Write(int,void *,size_t);
void *Malloc(size_t);
//----------------------------------------------------------------
//------------------------readn.c---------------------------------
ssize_t readn(int,void *,size_t);
ssize_t Readn(int,void *,size_t);
//----------------------------------------------------------------
//------------------------wrapsock.c------------------------------
void Bind(int,const struct sockaddr *,socklen_t);
void Listen(int,int);
int Socket(int,int,int);
int Accept(int,struct sockaddr *,socklen_t *);
void Setsockopt(int,int,int,const void *,socklen_t);
//----------------------------------------------------------------
//------------------------signal.c--------------------------------
Sigfunc * Signal(int, Sigfunc *);
//----------------------------------------------------------------
//------------------------readline.c------------------------------
ssize_t Readline(int, void *, size_t);
ssize_t readlinebuf(void **);
//----------------------------------------------------------------
//------------------------writen.c--------------------------------
void Writen(int fd, void *ptr, size_t nbytes);
//----------------------------------------------------------------
//------------------------sigchldwaitpid.c------------------------
void sig_chld(int);
void sig_int(int);
void web_child(int);
//----------------------------------------------------------------
//------------------------pr_cpu_time.c---------------------------
void pr_cpu_time(void);
//----------------------------------------------------------------
//------------------------web_child.c-----------------------------
void web_child(int);
//----------------------------------------------------------------
//------------------------tcp_listen.c----------------------------
int Tcp_listen(const char *, const char *, socklen_t *);
//----------------------------------------------------------------
int tcp_connect(const char *,const char *);
int Tcp_connect(const char *,const char *);
pid_t Fork(void);
void err_quit(const char *, ...);
void err_sys(const char *, ...);
//------------------------sctp_pdapircv.c-------------------------
uint8_t *pdapi_recvmsg(int,int *,SA *,int *,struct sctp_sndrcvinfo *,int *);
//----------------------------------------------------------------
//================================================================

#endif
