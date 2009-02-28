//===============================include files==================================
#include<stdlib.h>
#include<stdio.h>
#include<winsock2.h>
//==============================================================================

//================================define variable===============================
#define WSVERS MAKEWORD(2,0)                      //make a word type
#define PORT 21                                   // control msg
#define SD_PORT 22                                // transfer channel, (not need?)
#define FILEBUFFERSIZE 1024                       // each file split to 
//==============================================================================

//================================global variable===============================
SOCKET ClientSock;                                // master for listening
//==============================================================================

//================================function phototypes===========================
SOCKET ConnectToServer(char *);                   // make a passive socket
SOCKET ServerSock;
int RecvMsg(SOCKET ,char*);                       // Receive message from socket
int SendMsg(SOCKET,char*);                        // Send a message to socket

void RecvFile(SOCKET ,char*,char*);               // send a file from server
void RecvThread();                                // a thread for receive msg
//==============================================================================

 int main()
 {
     //variable
     char ipaddr[15];
     char recvbuffer[1024];
     char sendbuffer[1024];
     char filebuffer[1024];
     char filename[100];
     struct sockaddr_in SockAddr;
     int AddrLen = sizeof(SockAddr);
     WSADATA wsadata;
     
     //initiallize
     memset(ipaddr,0,sizeof(ipaddr));
     if(WSAStartup(WSVERS, &wsadata)!=0)
     {
         printf("WSAStartup failed!!\n");
         exit(0);
     }     
     
     printf("enter the ip address: ");
     scanf("%s",ipaddr);
     
     ServerSock = ConnectToServer(ipaddr);
     printf("waiting for welcome....\n");
     RecvMsg(ServerSock,recvbuffer);
     printf("%s",recvbuffer);
     while(1)
     {
         RecvMsg(ServerSock,recvbuffer);
         printf("%s",recvbuffer);
         scanf("%s",sendbuffer);
         SendMsg(ServerSock,sendbuffer);
         if(strncmp(sendbuffer,"#getfile",strlen("#getfile")) == 0)
         { //receive file
             strcpy(filename,sendbuffer + strlen("#getfile "));
             if(strcmp(filename,"")!=0)
             {
                 printf("ready to receive....\n");
                 RecvFile(ServerSock,filebuffer,filename);
             }
         }
         else if(strcmp(sendbuffer,"#exit") == 0)break;
         
         else
         {
             RecvMsg(ServerSock,recvbuffer);
             printf("%s\n",recvbuffer);
         }         
     }
     closesocket(ServerSock);
     WSACleanup();
     printf("End!\n");
     system("pause");
     return 0;    
 }

//===========================function impletatiom===============================
SOCKET ConnectToServer(char *ipaddr)
{
    SOCKET ServerSock;
    struct sockaddr_in ServAddr;
    int TransportType;
    
    //setting
    memset(&ServAddr , 0 , sizeof(ServAddr));
    ServAddr.sin_family = PF_INET;
    ServAddr.sin_addr.s_addr = inet_addr(ipaddr);
    ServAddr.sin_port = htons(PORT);
    TransportType = SOCK_STREAM;                                  //tcp type
    
    //open a socket
    ServerSock = socket(PF_INET, TransportType, IPPROTO_TCP);
    if(ServerSock == INVALID_SOCKET)
    {
        printf("Error to open socket!!!\n");
        exit(0);
    }
    
    if(connect(ServerSock,(struct sockaddr far*)&ServAddr,sizeof(ServAddr)) == SOCKET_ERROR)
    {
        printf("Error to Connect socket!!!\n");
        exit(0);
    }
    return ServerSock;
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

void RecvFile(SOCKET skt,char *filebuffer, char *filename)
{
    // true for seccessful, false for unseccessful
    FILE *fpt = fopen(filename,"w");
    
    RecvMsg(skt,filebuffer);
    while(strcmp(filebuffer,"#over") != 0)
    {
        fwrite(filebuffer,sizeof(filebuffer),sizeof(filebuffer),fpt);
        RecvMsg(skt,filebuffer);
    }
    fclose(fpt);
    //return 't';
}
//==============================================================================

