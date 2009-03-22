#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>

#ifndef SCTP_API_H
#define SCTP_API_H

int Sctp_bindx(int sockfd,const struct sockaddr *addrs,int addrcnt,int flags);

int Sctp_connectx(int sockfd,const struct sockaddr *addrs,int addrcnt);

int Sctp_getpaddrs(int sockfd,sctp_assoc_t id,struct sockaddr **addrs);

void Sctp_freepaddrs(struct sockaddr *addrs);

int Sctp_getladdrs(int sockfd,sctp_assoc_t id,struct sockaddr **addrs);

int Sctp_freeladdrs(struct sockaddr *addrs);

ssize_t Sctp_sendmsg(int sockfd,const void *msg,size_t msgsz,const struct sockaddr *to,socklen_t tolen,uint32_t ppid,uint32_t flags,uint16_t stream,uint32_t timetolive,uint32_t context);

ssize_t Sctp_recvmsg(int sockfd,void *msg,size_t msgsz,struct sockaddr *from,socklen_t *fromlen,struct sctp_sndrcvinfo *sinfo,int *msg_flags);

int Sctp_opt_info(int sockfd,sctp_assoc_t assoc_id,int opt,void *arg,socklen_t *siz);

int Sctp_peeloff(int sockfd,sctp_assoc_t id);


#endif
