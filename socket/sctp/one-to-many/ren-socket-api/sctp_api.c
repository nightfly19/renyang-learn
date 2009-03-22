#include "sctp_api.h"

int Sctp_bindx(int sockfd,const struct sockaddr *addrs,int addrcnt,int flags) {
	return sctp_bindx(sockfd,addrs,addrcnt,flags);
}

int Sctp_connectx(int sockfd,const struct sockaddr *addrs,int addrcnt) {
	return sctp_connectx(sockfd,addrs,addrcnt);
}

int Sctp_getpaddrs(int sockfd,sctp_assoc_t id,struct sockaddr **addrs) {
	return sctp_getpaddrs(sockfd,id,addrs);
}

void Sctp_freepaddrs(struct sockaddr *addrs) {
	sctp_freepaddrs(addrs);
}

int Sctp_getladdrs(int sockfd,sctp_assoc_t id,struct sockaddr **addrs) {
	return sctp_getladdrs(sockfd,id,addrs);
}

int Sctp_freeladdrs(struct sockaddr *addrs) {
	return sctp_freeladdrs(addrs);
}

ssize_t Sctp_sendmsg(int sockfd,const void *msg,size_t msgsz,const struct sockaddr *to,socklen_t tolen,uint32_t ppid,uint32_t flags,uint16_t stream,uint32_t timetolive,uint32_t context) {
	return sctp_sendmsg(sockfd,msg,msgsz,to,tolen,ppid,flags,stream,timetolive,context);
}

ssize_t Sctp_recvmsg(int sockfd,void *msg,size_t msgsz,struct sockaddr *from,socklen_t *fromlen,struct sctp_sndrcvinfo *sinfo,int *msg_flags) {
	return sctp_recvmsg(sockfd,msg,msgsz,from,fromlen,sinfo,msg_flags);
}

int Sctp_opt_info(int sockfd,sctp_assoc_t assoc_id,int opt,void *arg,socklen_t *siz) {
	return sctp_opt_info(sockfd,assoc_id,opt,arg,siz);
}

int Sctp_peeloff(int sockfd,sctp_assoc_t id) {
	return sctp_peeloff(sockfd,id);
}
