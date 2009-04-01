//===========================include header===========================
#include <unp.h>
#include <time.h>
//====================================================================

int main(int argc,char **argv)
{
	int listenfd,connfd;
	struct sockaddr_in servaddr;
	char buff[MAXLINE];
	time_t ticks;
	
	listenfd = socket(AF_INET,SOCK_STREAM,0);
	if (listenfd !=0) {
		fprintf(stderr,"socket error\n");
		exit(-1);
	}

	return 0;
}
