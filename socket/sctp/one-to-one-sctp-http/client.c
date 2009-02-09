//===============================include files==================================
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netinet/sctp.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<unistd.h>
//==============================================================================

//================================define variable===============================
#define HTTP_PORT 8888                            // transfer channel, (not need?)
#define FILEBUFFERSIZE 1024                       // each file split to 
#define MAXFILETRANSFER 5                         // max file transfer
//==============================================================================

//================================global variable===============================
int ClientSock;                                   // master for listening
//==============================================================================

//================================function phototypes===========================
int ConnectToServer(char *);                      // make a passive socket
int ServerSock;
int RecvMsg(int,char *);                          // Receive message from socket
int SendMsg(int,char *);                          // Send a message to socket

void RecvFiles(int,char *);                       // send a file from server
//==============================================================================

 int main()
 {
     //variable
     char ipaddr[15];
     char recvbuffer[FILEBUFFERSIZE];
     char sendbuffer[FILEBUFFERSIZE];
     char filebuffer[FILEBUFFERSIZE];
     char *filelist;
     struct sockaddr_in SockAddr;
     int AddrLen = sizeof(SockAddr);
     
     //initiallize
     memset(ipaddr,0,sizeof(ipaddr));   
     
     printf("enter the ip address: ");
     // gets(ipaddr);
     fgets(ipaddr,sizeof(ipaddr),stdin);
     
     ServerSock = ConnectToServer(ipaddr);
     printf("waiting for welcome....\n");
     RecvMsg(ServerSock,recvbuffer);
     printf("%s",recvbuffer);
     while(1)
     {
         if(RecvMsg(ServerSock,recvbuffer)<0)break;
         if(strcmp(recvbuffer,"Full\n") == 0)break;
         printf("%s",recvbuffer);
         memset(sendbuffer,0,sizeof(sendbuffer));
         // gets(sendbuffer);
	 fgets(sendbuffer,sizeof(sendbuffer),stdin);

         SendMsg(ServerSock,sendbuffer);
         
         //--------------------http server modify this part---------------------
         if(strncmp(sendbuffer,"#getfiles",strlen("#getfiles")) == 0)
         { //receive files
             filelist = (char*)malloc(sizeof(char)*(strlen(sendbuffer)-strlen("#getfiles ")));
             strcpy(filelist,sendbuffer + strlen("#getfiles "));
             if(strcmp(filelist,"")!=0)
             {
                 printf("Receiving....\n");
                 RecvFiles(ServerSock,filelist);
                 printf("Finish!!\n");
             }
         }
         //---------------------------------------------------------------------
         else if(strncmp(sendbuffer,"#exit",strlen("#exit")) == 0)break;
         
         else
         {
            RecvMsg(ServerSock,recvbuffer);
	    printf("%s\n",recvbuffer);
         }        
         
         bzero(recvbuffer,sizeof(recvbuffer));
     }
     close(ServerSock);
     printf("End!\n");
     return 0;    
 }

//===========================function impletatiom===============================
int ConnectToServer(char *ipaddr)
{
    int ServerSock;
    struct sockaddr_in ServAddr;
    int TransportType;
    int ret;
    struct sctp_event_subscribe events;
    struct sctp_initmsg initmsg;


    //setting
    memset(&ServAddr , 0 , sizeof(ServAddr));
    ServAddr.sin_family = PF_INET;
    ServAddr.sin_addr.s_addr = inet_addr(ipaddr);
    ServAddr.sin_port = htons(HTTP_PORT);
    TransportType = SOCK_STREAM;                            
    
    //open a socket
    ServerSock = socket(PF_INET, TransportType, IPPROTO_SCTP);
    if(ServerSock < 0)
    {
        printf("Error to open socket!!!\n");
        exit(0);
    }
    
    bzero(&initmsg,sizeof(initmsg));
    initmsg.sinit_num_ostreams = MAXFILETRANSFER;
    initmsg.sinit_max_instreams = MAXFILETRANSFER;
    initmsg.sinit_max_attempts = MAXFILETRANSFER - 1;
    setsockopt(ServerSock,IPPROTO_SCTP,SCTP_INITMSG,&initmsg,sizeof(initmsg));
    if(connect(ServerSock,(struct sockaddr *)&ServAddr,sizeof(ServAddr)) < 0)
    {
        printf("Error to Connect socket!!!\n");
        exit(0);
    }

    // Enable receipt of SCTP Snd/Rcv Data via sctp_recvmsg
    bzero(&events,sizeof(events));
    events.sctp_data_io_event = 1;
    ret=setsockopt(ServerSock,132,SCTP_EVENTS,(const void*)&events,sizeof(events));
    printf("ret:%d\n",ret);
    return ServerSock;
}

int RecvMsg(int skt, char *msg)
{
    bzero(msg,FILEBUFFERSIZE);
    return recv(skt,msg,FILEBUFFERSIZE,0);
}

int SendMsg(int skt, char *msg)
{
    return send(skt,msg,strlen(msg),0);
}

void scanchar(char *buffer, char ch,int *pointer,int len)
{
    for(*pointer ; *pointer < len; (*pointer)++)
    {
        if(buffer[*pointer]==ch)break;
    }
}
//==========================multistream function=============================
void RecvFiles(int skt, char *filelist)
{
    int st_ch,end_ch;
    int pointer=0;
    char recvbuffer[FILEBUFFERSIZE];
    char ch;
    char filename[100];
    int recvbyte = 0;
    int i,flags;
    int spacebar_count = 0; //file number
    struct sctp_sndrcvinfo srinfo;
    FILE *fpt_list[MAXFILETRANSFER];

    // count the files
    for(i=0;i<strlen(filelist);i++)
    {
        ch = filelist[i];
        if(ch == ' ' && filelist[i+1]!=' ')spacebar_count++;
    }
    spacebar_count++;
    // extract the file names and open files to write
    for(i=0;i<spacebar_count;i++)
    {
        st_ch = pointer;
        scanchar(filelist,' ',&pointer,strlen(filelist));
        end_ch = pointer++;
        if(end_ch == strlen(filelist)) end_ch++;
        bzero(filename,sizeof(filename));
        strncpy(filename,filelist+st_ch,end_ch-st_ch);
        fpt_list[i] = fopen(filename,"wb");
        if(fpt_list[i] == NULL)
        {
            printf("File open error!!\n");
            return ;
        }
    }

    // receive the chunk and write to files
    while(1)
    {        
        memset(recvbuffer,0,FILEBUFFERSIZE);
        memset(&srinfo,1,sizeof(srinfo));
        recvbyte = sctp_recvmsg(skt,(void*)recvbuffer,
                                sizeof(recvbuffer),(struct sockaddr*)NULL,0,&srinfo,&flags);
        if(strcmp(recvbuffer,"#over#") == 0)break;

        for(i = 0;i<spacebar_count;i++)
        if(srinfo.sinfo_stream == i)
        {
            fwrite(recvbuffer,sizeof(char),recvbyte,fpt_list[i]);
        }
    }
   
    // close the files
    for(i=0;i<spacebar_count;i++)
    fclose(fpt_list[i]);
}
//==============================================================================

