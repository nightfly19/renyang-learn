#ifndef UNP_H
#define UNP_H

//========================include header==========================
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
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
//================================================================

//========================typedef=================================
typedef void Sigfunc(int);
//================================================================

//========================define function=========================
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
//================================================================

#endif
