#include "unp.h"

char *sctp_getprim(int sockfd,int assoc_id)
{
	struct sockaddr_in *in_addr;
	struct sockaddr_in6 *in6_addr;
	char addr_buf[INET6_ADDRSTRLEN];
	struct sockaddr *saddr = NULL;
	struct sctp_prim prim;
	socklen_t prim_len = sizeof(prim);
	int ret;
	const char *ap = NULL;
	char *ret_ap = NULL;

	prim.ssp_assoc_id = assoc_id;
	ret = getsockopt(sockfd,IPPROTO_SCTP,SCTP_PRIMARY_ADDR,&prim,&prim_len);
	if (ret < 0) {
		err_ret("Error: sctp_getprim");
		return NULL;
	}

	// 取出ip struct
	saddr = (struct sockaddr *)&prim.ssp_addr;
	if (AF_INET == saddr->sa_family) {
		in_addr = (struct sockaddr_in *)&prim.ssp_addr;
		ap = inet_ntop(AF_INET,&in_addr->sin_addr,addr_buf,INET6_ADDRSTRLEN);
	} else if (AF_INET6 == saddr->sa_family) {
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
