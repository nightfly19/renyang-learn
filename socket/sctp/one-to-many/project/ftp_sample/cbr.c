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
// renyang - 表示目前使用的stream個數
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

// renyang - 在結束程式之前,用來結束所有的設定
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


// renyang - 等待使用著輸入資料
// renyang - 若server端有使用者輸入字串,則停止程式
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
	// renyang - 是因為變數的scope，因此必需要使用指標來建立address嗎??
	struct sockaddr_in *addrs;
    struct sctp_event_subscribe event;

	fd = -1;
    // renyang - 驚!!是建立one-to-one的形式, 這樣也可以使用event的特性喔
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
	// 把socket bind在多個address上面
	result = sctp_bindx(fd, (struct sockaddr *)addrs, ipCnt, SCTP_BINDX_ADD_ADDR);	

	if(result < 0)
	{
		perror("bind failed");
		return -1;
	}
		
		
	// 設定觀察所有的事件
	memset(&event, 0, sizeof(event));
		
	event.sctp_data_io_event 			= 1;
	event.sctp_association_event		= 1;
	event.sctp_address_event			= 1;
	event.sctp_send_failure_event		= 1;
	event.sctp_peer_error_event			= 1;
	event.sctp_shutdown_event			= 1;
	event.sctp_partial_delivery_event	= 1;
#ifdef UN_MOD
	event.sctp_adaptation_layer_event	= 1;
#else
	event.sctp_adaption_layer_event		= 1;
#endif

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

	// renyang - client端應該不需要bind??
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

	// renyang - 連線到多個server端, 但是，一開始不是宣告one-to-one的sctp嗎?為什麼可以使用sctp_connectx??
	result = sctp_connectx(fd, (struct sockaddr*)addrs, rIpCnt);

	if(result < 0)
	{
		perror("sctp_connectx");
		close(fd);
		return -1;
	}
	
	return fd;
}

// renyang - 建立server端的socket
// renyang - ips:儲放本地端ip的地址. ipCnt:本地端的ip個數. port:本地端使用的port. lCnt: 本地端(server端)要建立listen的queue個數
int clisten(char** ips, int ipCnt, int port, int lCnt)
{
	int fd;

	fd = cbind(ips, ipCnt, port);
	
	if(fd > 0)
	{
		// renyang - 建立多個等待client連線的queue
		if(listen(fd, lCnt) > -1)
		{
			return fd;
		}
	}
	
	return -1;
}



// renyang - 接收對方回傳回來的資料, parms表示對方的socket file descriptor
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
		if(msg_flags & MSG_NOTIFICATION)	// renyang - 若這一個成立的話, 則表示是事件, 目前不處理啦
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

// renyang - 參數資料的函式, parms為對方的socket file descriptor
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
		// renyang - 所以, 只有stream 1和stream 2在使用
		for(i=1; i<numOfStream && keepSending; i++)
		{
			printf("send sid: %d, size: %d\n", i, sizeof(buf));
			memset(buf, sizeof(buf), 49);

			sz = sctp_sendmsg(
				*fd,	// renyang - local socket file descripter
				buf,	// renyang - the pointer of the data will be sent
				sizeof(buf),	// renyang - the length of the data
				0,	// renyang - peer endpoint struct sockaddr
				0,	// renyang - length of the address stored previous field
				0,	// renyang - the payload protocol identifier that will be passed with the data chunk
				0,	// renyang - flags, SCTP options
				i,	// stream id
				10,	// renyang - time-to-live
				0	// renyang - specified context
			);

			if(sz == -1)
			{
				keepSending = 0;
			}
		}
	}

	exitProgram();
	// renyang - 必需使用pthread_exit(), 則只會結束目前這一個thread
	// renyang - 若使用exit(), 則會結束所有的目前這一個process的thread包含process
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
	// renyang - 當有資料可以讀時, 就跳出迴圈
	while (select(local_fd+1, &fdset, 0, 0, 0) <= 0){ continue;}
		
	len = sizeof(cli_addr);
	// renyang - 因為是one-to-one的型態，因此必需要有accept()來等待client端的連線
	// renyang - 回傳的是相對於peer端的socket file descriptor
	remote_fd = accept(local_fd, (struct sockaddr*)&cli_addr, (socklen_t *)&len);

	if (remote_fd > 0)
	{
		printf(
			"\n Connected from %s\n",
			inet_ntoa(cli_addr.sin_addr)
		);
			

		// send data thread
		// renyang - 先初始化一個pthread的參數attr
		pthread_attr_init(&attr);
		// renyang - 設定pthread_attr_t attr的PTHREAD_CREATE_JOINABLE的特性
		// renyang - 設定attr為可以設定為同步,應該就是說可以使用pthread_join()來等待指定的process
		// renyang - 預設本來就設定可以pthread_join()
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
		// renyang - 建立一個pthread,pthread儲放在sendDataThread
		// renyang - 屬性為attr,執行的function為sendData,函數的參數為remote_fd
		// renyang - 目前這一個範例程式的server端只會執行到目前這一個地方，就一直在sendData內loop啦
		pthread_create(&sendDataThread, &attr, sendData, &remote_fd);
		// renyang - 當程式跑到這裡時,表示thread的function也跑完了，可以把attr刪除掉了
		pthread_attr_destroy(&attr);

		// renyang - 以下的資料server端在目前這一個程式並不會用到
		// receive data thread
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
		pthread_create(&receiveDataThread, &attr, receiveData, &remote_fd);
		pthread_attr_destroy(&attr);
		

		// renyang - main函式必需等待sendDataThread執行完
		pthread_join(sendDataThread, NULL);
		// renyang - main函式必需等待receiveDataThread執行完
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
						localPort)
						) < 1 && programAlive )
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
		// renyang - 建立10個儲存本地端ip的陣列
		localIps[i] = malloc(16);
	}
	for(i=0; i< 10; i++)
	{
		// renyang - 建立10個儲存peer端的ip陣列
		remoteIps[i] = malloc(16);
	}	
	// renyang - 記錄儲存的本地端ip個數
	localIpCnt = 0;
	// renyang - 記錄儲存的peer端ip個數
	remoteIpCnt = 0;

	// renyang - 設定第一個本地端的ip位址
	localIps[localIpCnt++] = "127.0.0.1";
	// renyang - 設定第一個peer端的ip位址
	remoteIps[remoteIpCnt++] = "127.0.0.1";
	
	// renyang - 目前程式在運作
	programAlive = 1;

	// renyang - server端
	if(argc > 1 && strcmp(argv[1], "server") == 0)
	{
		// renyang - 本地端使用的port號
		localPort = 1557;
		// renyang - 遠端使用的port號
		remotePort = 1558;
		// renyang - server端執行的程式
	   	sender();
	}
	else	// renyang - client端
	{
		localPort = 1558;
		remotePort = 1557;
		receiver();
	}

	return 0;
}
