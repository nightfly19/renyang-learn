#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netinet/sctp.h>
#include <sys/uio.h>
#include <memory.h>
#include <unistd.h>


#include <pthread.h>
#include <time.h>


int remote_fd		= -1;
int local_fd		= -1;
int keepSending 	= 0;
int keepReceiving 	= 0;
int programAlive	= 1;
int numOfStream		= 3;

pthread_t sendDataThread;
pthread_t receiveDataThread;
pthread_t waitForUserInputThread;

char* localIps[10];
int localIpCnt;
int localPort;
char* remoteIps[10];
char remoteIpCnt;	
int remotePort;

void exitProgram()
{
	keepSending 	= 0;
	keepReceiving 	= 0;
	programAlive 	= 0;

	if(local_fd != 0)
	{
		close(local_fd);
		local_fd = 0;
	}
	if(remote_fd != 0)
	{
		close(remote_fd);
		remote_fd = 0;
	}

	exit(0);
}


void* waitForUserInput(void *null)
{
	fgetc(stdin);
	printf("Exit wait for user input\n");
	exitProgram();
}


int cbind(char** ips, int ipCnt, int port)
{
	int fd;
	int i;
	int result;
	struct sockaddr_in *addrs;
    struct sctp_event_subscribe event;

	fd = -1;
    if ((fd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP)) == -1)
    {
		perror("Create socket failed");
		return -1;
    }
	
	// bind any address
	if(ipCnt < 1)
	{
		addrs = (struct sockaddr_in*) malloc(sizeof(struct sockaddr_in));	
		addrs[0].sin_family 		= AF_INET;
		addrs[0].sin_port 			= htons(port);
		addrs[0].sin_addr.s_addr 	= INADDR_ANY;
		ipCnt = 1;
		
	}
	// bind given list of address
	else
	{
		addrs = (struct sockaddr_in*) malloc(sizeof(struct sockaddr_in)*ipCnt);
		for(i=0; i<ipCnt; i++)
		{
			addrs[i].sin_family 		= AF_INET;
			addrs[i].sin_port 			= htons(port);
			addrs[i].sin_addr.s_addr 	= inet_addr(ips[i]);
		}

	}
	result = sctp_bindx(fd, (struct sockaddr *)addrs, ipCnt, SCTP_BINDX_ADD_ADDR);	

	if(result < 0)
	{
		perror("bind failed");
		return -1;
	}
		
		
	memset(&event, 0, sizeof(event));
		
	event.sctp_data_io_event 			= 1;
	event.sctp_association_event		= 1;
	event.sctp_address_event			= 1;
	event.sctp_send_failure_event		= 1;
	event.sctp_peer_error_event			= 1;
	event.sctp_shutdown_event			= 1;
	event.sctp_partial_delivery_event	= 1;
	event.sctp_adaptation_layer_event	= 1;

	if(setsockopt(fd, IPPROTO_SCTP, SCTP_EVENTS, &event, sizeof(event)) != 0)
	{
		perror("set event failed");
	}


	return fd;
}


int cconnect(char** rIps, int rIpCnt, int rPort, char** lIps, int lIpCnt, int lPort)
{
	int fd;
	int i;
	int result;
	struct sockaddr_in *addrs;

	fd 			= -1;

	fd = cbind(lIps, lIpCnt, lPort);

	if(fd < 0)
		return -1;

	if(rIpCnt < 1)
	{
		addrs = (struct sockaddr_in*) malloc(sizeof(struct sockaddr_in));
		addrs[0].sin_family 		= AF_INET;
		addrs[0].sin_port 			= htons(rPort);
		addrs[0].sin_addr.s_addr 	= inet_addr("127.0.0.1");	
		rIpCnt = 1;
	}
	else
	{
		addrs = (struct sockaddr_in*) malloc(sizeof(struct sockaddr_in)*rIpCnt);
		for(i=0; i<rIpCnt; i++)
		{
			addrs[i].sin_family 		= AF_INET;
			addrs[i].sin_port 			= htons(rPort);
			addrs[i].sin_addr.s_addr 	= inet_addr(rIps[i]);
		}
	}

	result = sctp_connectx(fd, (struct sockaddr*)addrs, rIpCnt);

	if(result < 0)
	{
		perror("sctp_connectx");
		close(fd);
		return -1;
	}
	
	return fd;
}

int clisten(char** ips, int ipCnt, int port, int lCnt)
{
	int fd;

	fd = cbind(ips, ipCnt, port);
	
	if(fd > 0)
	{
		if(listen(fd, lCnt) > -1)
		{
			return fd;
		}
	}
	
	return -1;
}



void* receiveData(void *parms)
{
	int sz;
	char buf[10240];
	struct sctp_sndrcvinfo sri;
	int msg_flags;
	int *fd;
	keepReceiving	= 1;
	fd 				= (int*)parms;

	while(keepReceiving)
	{
		sz = sctp_recvmsg(*fd, buf, sizeof(buf), 0, 0, &sri, &msg_flags);
		if(msg_flags & MSG_NOTIFICATION)
		{
			continue;
		}

		if(sz < 1)
		{
			keepReceiving = 0;
		}
		else if(sz == 0)
		{
			printf("Good!!! receive size is 0\n");	
		}
		else
		{
			printf(
				"receive sid: %d, size: %d\n", 
				sri.sinfo_stream, 
				sz
			);
		}
	}

	exitProgram();
	pthread_exit(NULL);
}

void* sendData(void *parms)
{

	int sz, i;
	char buf[10240];
	struct sctp_sndrcvinfo sri;
	int msg_flags;
	int *fd;
	fd 				= (int*)parms;

	keepSending		= 1;
	while(keepSending)
	{
		for(i=1; i<numOfStream && keepSending; i++)
		{
			printf("send sid: %d, size: %d\n", i, sizeof(buf));
			memset(buf, sizeof(buf), 49);

			sz = sctp_sendmsg(
				*fd,
				buf,
				sizeof(buf),
				0,
				0,
				0,
				0,
				i,	// stream id
				10,
				0
			);

			if(sz == -1)
			{
				keepSending = 0;
			}
		}
	}

	exitProgram();
	pthread_exit(NULL);

}

void sender()
{
    int sz, len, msg_flags;
    struct sockaddr_in sin[1], cli_addr;
    struct sctp_event_subscribe event;
    struct sctp_sndrcvinfo sri;
    fd_set fdset;
    pthread_attr_t attr;
    
    
	pthread_create(&waitForUserInputThread, 0, waitForUserInput, 0);
	if((local_fd = clisten(localIps, localIpCnt, localPort, 1)) < 0)
	{
		exitProgram();
		return ;
	}
	printf("Waiting for associations at port %d...\n", localPort);

	FD_ZERO(&fdset);
	FD_SET(local_fd, &fdset);
	while (select(local_fd+1, &fdset, 0, 0, 0) <= 0){ continue;}
		
	len = sizeof(cli_addr);
	remote_fd = accept(local_fd, (struct sockaddr*)&cli_addr, (socklen_t *)&len);

	if (remote_fd > 0)
	{
		printf(
			"\n Connected from %s\n",
			inet_ntoa(cli_addr.sin_addr)
		);
			

		// send data thread
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
		pthread_create(&sendDataThread, &attr, sendData, &remote_fd);
		pthread_attr_destroy(&attr);

		// receive data thread
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
		pthread_create(&receiveDataThread, &attr, receiveData, &remote_fd);
		pthread_attr_destroy(&attr);
		

		pthread_join(sendDataThread, NULL);
		pthread_join(receiveDataThread, NULL);
		
	}
	else
	{
		perror("remote_fd < 0");
	}

	
	close(remote_fd);
	close(local_fd);
	printf("CBR Sender Exits\n");
}

void receiver()
{
	int sz, len, msg_flags;
	struct sctp_event_subscribe event;
	char buf[256];
	struct sctp_sndrcvinfo sri;

	int rt;
	pthread_attr_t attr;


    pthread_create(&waitForUserInputThread, 0, waitForUserInput, 0);

	remote_fd = -1;

	printf("\nConnecting to remote sender [%s:%d] !!\n",
		remoteIps[0],
		remotePort
	);

	while (	(remote_fd = cconnect(
						remoteIps, 
						remoteIpCnt, 
						remotePort, 
						localIps, 
						localIpCnt, 
						localPort)) < 1 && 
						programAlive )
	{
		sleep(1);		
	}

	if(!programAlive)
	{
		printf("Exit the program\n");
		return;
	}
	
	if(remote_fd < 0 )
	{

		printf("\nUnable to connect to remote sender [%s:%d] !!",
			remoteIps[0],
			remotePort
		);	
		
		return;
	}
	
	printf("\nConnect to [%s:%d]\n",
		remoteIps[0],
		remotePort
	);

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_create(&receiveDataThread, &attr, receiveData, &remote_fd);
	pthread_attr_destroy(&attr);

	pthread_join(receiveDataThread, NULL);	

	close(remote_fd);
	printf("CBR Receiver Exits\n");	
}



int main(int argc, char **argv)
{

	int i;

	for(i=0; i< 10; i++)
	{
		localIps[i] = malloc(16);
	}
	for(i=0; i< 10; i++)
	{
		remoteIps[i] = malloc(16);
	}	
	localIpCnt = 0;
	remoteIpCnt = 0;

	localIps[localIpCnt++] = "127.0.0.1";
	remoteIps[remoteIpCnt++] = "127.0.0.1";
	
	programAlive = 1;

	if(argc > 1 && strcmp(argv[1], "server") == 0)
	{
		localPort = 1557;
		remotePort = 1558;
	   	sender();
	}
	else
	{
		localPort = 1558;
		remotePort = 1557;
		receiver();
	}

	return 0;
}
