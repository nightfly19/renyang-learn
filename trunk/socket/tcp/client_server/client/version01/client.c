//==========================include header==========================
#include "unp.h"
//==================================================================

//==========================main function===========================
int main(int argc,char **argv)
{
	int i,j,fd,nchildren,nloops,nbytes;
	pid_t pid;
	ssize_t n;
	char request[MAXLINE],reply[MAXN];

	if (argc != 6) {
		fprintf(stderr,"usage: client <hostname or IPaddr> <port> <#children> "
			"<#loops/child> <#bytes/request>\n");
		exit(-1);
	}
	nchildren = atoi(argv[3]);
	nloops = atoi(argv[4]);
	nbytes = atoi(argv[5]);
	snprintf(request,sizeof(request),"%d\n",nbytes);	// newline at end

	for (i=0;i<nchildren;i++) {
		if ((pid=fork())==0) {	// child
			for (j=0;j<nloops;j++) {
				fd = Tcp_connect(argv[1],argv[2]);
				Writen(fd,request,strlen(request));
				
				if ((n=read(fd,reply,nbytes)!=nbytes)) {
					fprintf(stderr,"server returned %d bytes",n);
					exit(-1);
				}
				close(fd);
			}
			printf("child %d done\n",i);
			exit(0);
		}
		// parent loops around to fork() again
	}
	while (wait(NULL)>0)	// now parent waits for all children
		;
	if (errno != ECHILD) {
		err_sys("wait error");
	}
	return 0;
}
//==================================================================
