#include	"unp.h"

void
sctpstr_cli(FILE *fp, int sock_fd, struct sockaddr *to, socklen_t tolen)
{
	struct sockaddr_in peeraddr;
	struct sctp_sndrcvinfo sri;
	char sendline[MAXLINE], recvline[MAXLINE];
	socklen_t len;
	int out_sz,rd_sz;
	int msg_flags;

	bzero(&sri,sizeof(sri));
	while (fgets(sendline, MAXLINE, fp) != NULL) {
		if(sendline[0] != '[') {
#ifdef SETPRIM
//----------------------------------------------------------------------
			// 轉換指定的association的primary address
			if (sri.sinfo_assoc_id != 0) {
				printf("You want to change the local primary address to %s",sendline);
				struct sockaddr_in servaddr;
				bzero(&servaddr,sizeof(servaddr));
				servaddr.sin_family = AF_INET;
				servaddr.sin_port = htons(SERV_PORT);
				// Inet_pton(AF_INET,sendline,&servaddr.sin_addr);
				servaddr.sin_addr.s_addr = inet_addr(sendline);	// 這一行也可以取代上面那一行
				sctp_setprim(sock_fd,sri.sinfo_assoc_id,(struct sockaddr_storage *) &servaddr);
			}
			else
//----------------------------------------------------------------------
#endif
				printf("Error, line must be of the form '[streamnum]text'\n");
			continue;
		}

		sri.sinfo_stream = strtol(&sendline[1],NULL,0);
		out_sz = strlen(sendline);
		Sctp_sendmsg(sock_fd, sendline, out_sz, 
			     to, tolen, 
			     0, 0,
			     sri.sinfo_stream,
			     0, 0);

		len = sizeof(peeraddr);
		rd_sz = Sctp_recvmsg(sock_fd, recvline, sizeof(recvline),
			     (SA *)&peeraddr, &len,
			     &sri,&msg_flags);
		printf("From str:%d seq:%d (assoc:0x%x):",
		       sri.sinfo_stream,sri.sinfo_ssn,
		       (u_int)sri.sinfo_assoc_id);
		printf("%.*s",rd_sz,recvline);
#ifdef GETPRIM
//----------------------------------------------------------------------
		// 列印出指定的association的primary address
		printf("the local primary address is : %s\n",sctp_getprim(sock_fd,(u_int)sri.sinfo_assoc_id));
//----------------------------------------------------------------------
#endif
	}
}
