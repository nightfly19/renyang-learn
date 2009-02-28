//===============================include files==================================
#include<stdlib.h>
#include<stdio.h>
#include<iostream>
#include<winsock2.h>
#include<process.h>
#include<dirent.h>
//==============================================================================

//================================define variable===============================
#define WSVERS MAKEWORD(2,0)                      //make a word type
#define PORT 21                                   // control msg
#define SD_PORT 22                                // transfer channel
#define MAXCONNECTION 500                         // max size of connections
#define AVAILCONNECTION 10                        // availiable connections
#define FILEBUFFERSIZE 1024                       // each file split to 

typedef struct SlaveSock_s
{
    SOCKET skt;
    struct SlaveSock_s *link;
}SlaveSock_t;
//==============================================================================

//================================global variable===============================
SOCKET MasterSock;                                // master for listening
SlaveSock_t *AvailSock;                           // slave for serving             
SlaveSock_t *ConnectedSock;                       // connected socket
SlaveSock_t *AcceptSock;                          // accepted connection put to
                                                  // connected socket list
int ConnectedCount;                               // record the connected sock 
                                                  // number
HANDLE GetSock_Semaphore;                         // for handle share data

HANDLE aThread[MAXCONNECTION];                     // for handle thread
DWORD ThreadID;
//==============================================================================

//================================function phototypes===========================
SOCKET MakePassiveSock(unsigned short int);       // make a passive socket
int RecvMsg(SOCKET , char*);                      // Receive command from socket
int SendMsg(SOCKET, char*);                       // Send a message to socket
void MakeSlavePool(int);                          // make a pool for slave
SlaveSock_t *GetSock();                           // get a sock from pool
void ReleaseSock(SlaveSock_t *);                  // release a sock to pool
//void PutToConnList(SlaveSock_t *);                // put a slave sock to 
                                                  // connected list

void SDFile(SOCKET ,char*);                       // send a file to slave
void ListFile(char*,char*);                       // list files in current path
                                                  // and store it into buffer

void CommunicateThread(SlaveSock_t *);            // a thread to serve
//==============================================================================

 int main()
 {
     //variable

     char msg[] = "WELCOME\n";
     char tmp[1024];
     char cont = 'y';
     int client_id;
     
     struct sockaddr_in SockAddr;
     int AddrLen = sizeof(SockAddr);
     WSADATA wsadata;
     
     //initiallize
     MakeSlavePool(MAXCONNECTION);
     ConnectedSock = NULL;
     AcceptSock = NULL;
     GetSock_Semaphore = CreateSemaphore(
                               NULL,
                               AVAILCONNECTION,
                               AVAILCONNECTION,
                               NULL);
     
     if(WSAStartup(WSVERS, &wsadata)!=0)
     {
         printf("WSAStartup failed!!\n");
         exit(0);
     }     
     
     /*
     * multithread for listening & mutex for handle synchronization
     */
     
     MasterSock = MakePassiveSock(PORT);
     printf("listening...\n");
     
     //it's a testing
     client_id = 0;
     while(cont == 'y')
     {
         if(ConnectedSock == NULL)
         {
             WaitForSingleObject(GetSock_Semaphore,INFINITE);             
             ConnectedSock = GetSock();
             AcceptSock = ConnectedSock;
             //printf("ConnectedSock = NULL\n");
         }
         else
         {
             AcceptSock = ConnectedSock;
             // move to tail of connected socket list
             while(AcceptSock->link != NULL) AcceptSock = AcceptSock->link;
             WaitForSingleObject(GetSock_Semaphore,INFINITE);             
             AcceptSock->link = GetSock();
             AcceptSock = AcceptSock->link;
         }
         AcceptSock->skt = accept(
                                  MasterSock,
                                  (struct sockaddr*)&SockAddr,
                                  &AddrLen);
         aThread[client_id] = CreateThread(
			                  NULL,
			                  0,
                              (LPTHREAD_START_ROUTINE)CommunicateThread,
			                  (LPVOID)AcceptSock,
		                      0,
	                          &ThreadID);
     }
     printf("End!\n");
     system("pause");
     return 0;    
 }

//===========================function impletatiom===============================
SOCKET MakePassiveSock(unsigned short int portnumber)
{
    SOCKET PassiveSock;
    struct sockaddr_in ServAddr;
    int TransportType;
    
    //setting
    memset(&ServAddr , 0 , sizeof(ServAddr));
    ServAddr.sin_family = PF_INET;
    ServAddr.sin_addr.s_addr = INADDR_ANY;
    ServAddr.sin_port = htons(portnumber);
    TransportType = SOCK_STREAM;                                  //tcp type
    
    //open a socket
    PassiveSock = socket(PF_INET, TransportType, IPPROTO_TCP);
    if(PassiveSock == INVALID_SOCKET)
    {
        printf("Error to open socket!!!\n");
        exit(0);
    }
    
    if(bind(PassiveSock,(struct sockaddr*)&ServAddr, sizeof(ServAddr)) == SOCKET_ERROR)
    {
        printf("Error to bind socket!!!\n");
        exit(0);
    }
    
    if(listen(PassiveSock,AVAILCONNECTION) == SOCKET_ERROR)
    {
        printf("Error to listen!!!\n");
        exit(0);
    }
    return PassiveSock;
}

int RecvMsg(SOCKET skt, char *msg)
{
    memset(msg,0,FILEBUFFERSIZE);
    return recv(skt,msg,FILEBUFFERSIZE,0);
}

int SendMsg(SOCKET skt, char *msg)
{
    return send(skt,msg,strlen(msg),0);
}

void MakeSlavePool(int n)
{
     int i;
     SlaveSock_t *mv;
     
     //make one SlaveSock_t first
     AvailSock = (SlaveSock_t*)malloc(sizeof(SlaveSock_t));
     AvailSock->link = NULL;
     mv = AvailSock;
     
     //make the rest SlaveSock_t
     for(i=1 ; i<n ; i++)
     {
         mv->link = (SlaveSock_t*)malloc(sizeof(SlaveSock_t));
         mv = mv->link;
         mv->link = NULL;
     }
}

/*SOCKET MakeSendFileSocket(SOCKET skt)
{
    SOCKET SendFileSock;
    struct sockaddr_in sdaddr;
    int TransportType;
    
    //setting
    memset(&sdaddr,0,sizeof(sdaddr));
    sdaddr.sin_family = PF_INET;
    //sdaddr.sin_addr.s_addr = skt.sin_addr.s_addr;   //need help
    sdaddr.sin_port = htons(SD_PORT);
    TransportType = SOCK_STREAM;
    
    //open a socket
    SendFileSock = socket(PF_INET,TransportType,IPPROTO_TCP);
    if(SendFileSock == INVALID_SOCKET)
    {
        printf("Error to open send file socket!!!\n");
        exit(0);
    }
    
    if(bind(SendFileSock,(struct sockaddr*)&sdaddr, sizeof(sdaddr)) == SOCKET_ERROR)
    {
        printf("Error to bind send file socket!!!\n");
        exit(0);
    }
    
    if(connect(SendFileSock,(struct sockaddr*)&sdaddr,sizeof(sdaddr)) != 0)
    {
        printf("Error to connect send file socket!!!\n");
        exit(0);
    }
    return SendFileSock;
    
}*/

SlaveSock_t *GetSock()
{
    SlaveSock_t *sock = AvailSock;
    AvailSock = AvailSock->link;
    sock->link = NULL;
    return sock;
}

void ReleaseSock(SlaveSock_t *slvskt)
{
    slvskt->link = AvailSock;
    AvailSock = slvskt;     
}

//not need?
/*void PutToConnList(SlaveSock_t *slvskt)
{
    slvskt->link = ConnectedSock ;
    ConnectedSock = slvskt;
}*/

void SDFile(SOCKET skt,char *filename)
{
    //i need to think about it
    char sdbuffer[FILEBUFFERSIZE];             //buffer size = 1024 bytes = 1 KB
    char end[7] = "#over#";
    int ReadByte = 0;
    FILE *sdfile = fopen(filename,"rb");
    if(sdfile == NULL)
    {
        printf("Client:<#%d> file<%s> open error!!\n",skt,filename);
        send(skt,end,strlen(end),0);
        return;
        // if no directory in local, error will occur
    }
    
    printf("Client:<#%d> sending file <%s>....\n",skt,filename);
    // need a socket to transfer
    // think about other appoach to send
    while( (ReadByte = fread(sdbuffer,sizeof(char),FILEBUFFERSIZE,sdfile)) >0 )	// 使用binary的方式讀取檔案
    {
        //printf("Client:<#%d> sending <%s> \n",skt,sdbuffer);
        //printf("readbyte = %d\n",ReadByte);
        send(skt,sdbuffer,ReadByte,0);	// 傳送資料
        memset(sdbuffer,0,FILEBUFFERSIZE);
    }
    fclose(sdfile);
    Sleep(500); // wait to finish
    send(skt,end,sizeof(end),0);	// 傳送結束的檔案字尾
    printf("Client:<#%d> Finish!! <%s>\n",skt,filename);
    
}

void ListFile(char *filesbuffer, char *currpath)
{
    DIR           *d;
    struct dirent *dir;
    d = opendir(currpath);
    if(d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            sprintf(filesbuffer+strlen(filesbuffer),"%s ", dir->d_name);
        }
        closedir(d);
    }
}

void CommunicateThread(SlaveSock_t *slvskt)
{
     char recvbuf[1024];                        // for sd/recv command
     char sdbuf[1024];                          // for sd/recv msg
     char sdfilename[20];                       // sending file storage
     char filelist[1024];                       // files in current dir
     char currpath[1024];                       // current path
     char buffer[1024];                         // a buffer for cd
     char buffer2[1024];                        // a buffer for temp

     memset(recvbuf,0,sizeof(recvbuf));
     memset(sdbuf,0,sizeof(sdbuf));
     memset(sdfilename,0,sizeof(sdfilename));
     memset(filelist,0,sizeof(filelist));
     memset(currpath,0,sizeof(currpath));
     memset(buffer,0,sizeof(buffer));
     strcpy(currpath,".");
     
     printf("Client:<#%d> login!!\n",slvskt->skt);
     memset(sdbuf,0,sizeof(sdbuf));
     //printf("msg size:%d\n",sizeof(msg));
     sprintf(sdbuf,"Welcome<your id:%d>!!\n",slvskt->skt);
     
     SendMsg(slvskt->skt,sdbuf);
     while(1)
     {
         SendMsg(slvskt->skt,"cmd> ");
         if(RecvMsg(slvskt->skt,recvbuf) == SOCKET_ERROR)break;
         printf("cmd> %s\n",recvbuf);
         if(recvbuf[0] != '#')
         {
             strcpy(sdbuf,"Command starting char must be '#'");
             SendMsg(slvskt->skt,sdbuf);
         }
         else if(strncmp(recvbuf,"#getfile",strlen("#getfile")) == 0)
         {
             memset(sdfilename,0,20);
             strcpy(sdfilename,recvbuf + strlen("#getfile "));  //extract the file name
             printf("Client:<#%d>: require file <%s>\n",slvskt->skt,sdfilename);
             memset(buffer2,0,FILEBUFFERSIZE);
             strcpy(buffer2, currpath);	// 取得目前的位置
             strcat(buffer2,"\\");	// 在目前的位置最後加上\\
             strcat(buffer2, sdfilename);
             SDFile(slvskt->skt,buffer2);	// 傳送檔案
         }
         else if(strncmp(recvbuf,"#list",strlen("#list")) == 0)
         {
             memset(filelist,0,sizeof(filelist));
             ListFile(filelist,currpath);
             SendMsg(slvskt->skt,filelist);
         }
         else if(strncmp(recvbuf,"#cd",strlen("#cd")) == 0)
         {
             memset(buffer,0,sizeof(buffer));
             strcpy(buffer,recvbuf + strlen("#cd "));
             if(strcmp(currpath,".") == 0 && strcmp(buffer,"..") != 0 
                && strlen(buffer)>=0 )
             {
                 strcat(currpath,"\\");
                 strcat(currpath,buffer);
             }
             else if(strcmp(currpath,".") != 0 && strcmp(buffer,"..") == 0
                     && strlen(buffer)>=0)
             {
                 *(strrchr(currpath,'\\')) = '\0';
             }
             else if(strcmp(currpath,".") == 0 && strcmp(buffer,"..") == 0
                     && strlen(buffer)>=0)
             {
                 //need to do nothing because the top directory is "."
             }
             memset(buffer,0,sizeof(buffer));
             ListFile(buffer,currpath);
             SendMsg(slvskt->skt,buffer);
         }
         else if(strcmp(recvbuf,"#exit") == 0)
         {
            break;                                //terminate the communication
         }
         else
         {
             SendMsg(slvskt->skt,"Command not support yet!");
         }
         //memset(recvbuf,0,FILEBUFFERSIZE);
     }
     
     ReleaseSemaphore(GetSock_Semaphore,1,NULL);
     printf("Client:<#%d> logout!!\n",slvskt->skt);
     ReleaseSock(slvskt);
     closesocket(slvskt->skt);
     
}
//==============================================================================

