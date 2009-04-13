#ifndef UNP_H
#define UNP_H

//========================include header==========================
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/wait.h>
#include <sys/un.h>	// for the struct sockaddr_un
#include <fcntl.h>	// for the fcntl function
//================================================================

//========================define variable=========================
#define MAXLINE 4096
#define LISTENQ 1024
#define SA struct sockaddr
#define SCTP_PDAPI_INCR_SZ 65535   /* increment size for pdapi when adding buf space */
#define SCTP_PDAPI_NEED_MORE_THRESHOLD 1024	/* need more space threshold */
#define SERV_MAX_SCTP_STRM	10	/* normal maximum streams */
#define	BUFFSIZE	8192	/* buffer size for reads and writes */
#define	SERV_PORT		 9877			/* TCP and UDP client-servers */
#define SERV_MORE_STRMS_SCTP 30
//================================================================

//========================typedef=================================
typedef void Sigfunc(int);
//================================================================

//========================define function=========================
//------------------------sctp_getnostrm.c------------------------
int sctp_get_no_strms(int,struct sockaddr *,socklen_t);
//----------------------------------------------------------------
//------------------------sctp_events.c---------------------------
void SctpTurnOnAllEvent(int);
void SctpTurnOffAllEvent(int);
//----------------------------------------------------------------
//------------------------sctp_displayevents.c--------------------
void print_notification(char *);
//----------------------------------------------------------------
//------------------------sctp_modify_hb.c------------------------
int heartbeat_action(int, struct sockaddr *, socklen_t, u_int);
//----------------------------------------------------------------
//------------------------sctp_addr_to_associd.c------------------
sctp_assoc_t sctp_address_to_associd(int,struct sockaddr *,socklen_t);
//----------------------------------------------------------------
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
//------------------------sctp_strcli_un.c------------------------
void sctpstr_cli_un(FILE *, int, struct sockaddr *, socklen_t);
//----------------------------------------------------------------
//------------------------sctp_bindargs.c-------------------------
int sctp_bind_arg_list(int, char **, int);
//----------------------------------------------------------------
//------------------------host_serv.c-----------------------------
struct addrinfo * Host_serv(const char *, const char *, int, int);
//----------------------------------------------------------------
//------------------------sctp_strcli_info.c----------------------
void sctpstr_cli_info(FILE *, int, struct sockaddr *, socklen_t);
//----------------------------------------------------------------
//------------------------sctp_check_notify.c---------------------
void check_notification(int,char *,int);
//----------------------------------------------------------------
//------------------------sctp_print_addrs.c----------------------
void sctp_print_addresses(struct sockaddr_storage *, int);
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
void Getsockopt(int,int,int,void *,socklen_t *);
//----------------------------------------------------------------
//------------------------wraplib.c-------------------------------
const char *Inet_ntop(int,const void *,char *,size_t);
void Inet_pton(int,const char *,void *);
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
//------------------------error.c---------------------------------
void err_quit(const char *, ...);
void err_sys(const char *, ...);
void err_ret(const char *, ...);
//----------------------------------------------------------------
//------------------------str_echo.c------------------------------
void str_echo(int sockfd);
//----------------------------------------------------------------
//------------------------sctp_pdapircv.c-------------------------
uint8_t *pdapi_recvmsg(int,int *,SA *,int *,struct sctp_sndrcvinfo *,int *);
//----------------------------------------------------------------
//------------------------sctp_prim_addr.c------------------------
char *sctp_getprim(int,int);
void sctp_setprim(int,int,struct sockaddr_storage *);
//----------------------------------------------------------------
//================================================================

#endif
