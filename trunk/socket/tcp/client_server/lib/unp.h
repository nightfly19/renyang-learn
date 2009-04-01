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
//================================================================

//========================define variable=========================
#define MAXLINE 4096
#define LISTENQ 1024
#define SA struct sockaddr
//================================================================

//========================define function=========================
//------------------------wrapunix.c------------------------------
void Close(int);
void Write(int,void *,size_t);
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
//----------------------------------------------------------------
int tcp_connect(const char *,const char *);
int Tcp_connect(const char *,const char *);
pid_t Fork(void);
void err_quit(const char *, ...);
void err_sys(const char *, ...);
//================================================================

#endif
