//==========================include header==========================
#include <unp.h>
//==================================================================

//==========================define variable=========================
#define MAXN 16384	// max # bytes to request from server
//==================================================================

//==========================main function===========================
int main(int argc,char **argv)
{
	int i,j,fd,nchildren,nloops,nbytes;
	pid_t pid;
	ssize_t n;
	char request[MAXLINE],reply[MAXN];

	if (argc != 6) {
		err_quit("usage: client <hostname or IPaddr> <port> <#children> "
			"<#loops/child> <#bytes/request>");
	}
	nchildren = atoi(argv[3]);
	nloops = atoi(argv[4]);
	nbytes = atoi(argv[5]);
	snprintf(request,sizeof(request),"%d\n",nbytes);	// newline at end

	return 0;
}
//==================================================================
