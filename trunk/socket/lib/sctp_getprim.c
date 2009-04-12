#include "unp.h"

char *sctp_getprim(int sockfd,int assoc_id)
{
	struct sockaddr_in *in_addr;
	struct sockaddr_in6 *in6_addr;
	char addr_buf[INET6_ADDRSTRLEN];
	struct sockaddr_storage *saddr = NULL;
	struct sctp_prim prim;
	socklen_t prim_len = sizeof(prim);
	int ret;
	const char *ap = NULL;
	char *ret_ap = NULL;

	prim.ssp_assoc_id = assoc_id;
	// 取得local端的primary address的資訊
	ret = getsockopt(sockfd,IPPROTO_SCTP,SCTP_PRIMARY_ADDR,&prim,&prim_len);
	if (ret < 0) {
		err_ret("Error: sctp_getprim");
		return NULL;
	}

	// 取出local的primary的address,原本的型態為struct sockaddr_storage
	// 來判斷sockaddr_storage是儲存哪一種ip address
	saddr = &prim.ssp_addr;
	if (AF_INET == saddr->ss_family) {	// 判斷是否為ipv4
		in_addr = (struct sockaddr_in *)&prim.ssp_addr;
		// Numeric to Presentation
		ap = inet_ntop(AF_INET,&in_addr->sin_addr,addr_buf,INET_ADDRSTRLEN);
	} else if (AF_INET6 == saddr->ss_family) {	// 判斷是否為ipv6
		in6_addr = (struct sockaddr_in6 *)&prim.ssp_addr;
		ap = inet_ntop(AF_INET6,&in6_addr->sin6_addr,addr_buf,INET6_ADDRSTRLEN);
	}
	if (!ap) {
		err_ret("Error: sctp_getprim");
		return NULL;
	}

	ret_ap = Malloc(sizeof(addr_buf));
	memcpy(ret_ap,addr_buf,sizeof(addr_buf));

	return ret_ap;
}

void sctp_setprim(int sockfd,int assoc_id,struct sockaddr_storage *addr)
{
	int ret=0;
	struct sctp_setprim prim;
	prim.ssp_assoc_id = assoc_id;
	memcpy(&prim.ssp_addr,addr,sizeof(struct sockaddr_storage));
	ret = setsockopt(sockfd,IPPROTO_SCTP,SCTP_PRIMARY_ADDR,&prim,sizeof(prim));
	if (ret < 0) {
		err_ret("Error: sctp_setprim");
		return ;
	}
}
