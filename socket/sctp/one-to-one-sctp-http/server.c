//===============================include files==================================
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include <netinet/in.h>
#include<netinet/sctp.h>
#include<pthread.h>
#include<semaphore.h>
//==============================================================================

//================================define variable===============================
#define HTTP_PORT 8888                            // transfer channel
#define MAXCONNECTION 500                         // max size of connections
#define AVAILCONNECTION 10                        // availiable connections
#define FILEBUFFERSIZE 1024                       // each file split to 
#define MAXFILETRANSFER 5                         // max files transfer

typedef struct SlaveSock_s
{
    int skt;
	int id;
    struct SlaveSock_s *link;
}SlaveSock_t;
//==============================================================================

//================================global variable===============================
int MasterSock;                                   // master for listening
SlaveSock_t *AvailSock;                           // slave for serving             
SlaveSock_t *AcceptSock;                          // accepted connection put to
                                                  // connected socket list
int ConnectedCount;                               // record the connected sock 
                                                  // number
sem_t GetSock_Semaphore;                          // for handle share data
sem_t SockCount_Semaphore;                        // for handle sock count

pthread_t aThread[AVAILCONNECTION];               // for handle thread
//==============================================================================

//================================function phototypes===========================
int MakePassiveSock(unsigned short int);          // make a passive socket
int RecvMsg(int , char*);                         // Receive command from socket
int SendMsg(int, char*);                          // Send a message to socket
void MakeSlavePool(int);                          // make a pool for slave
SlaveSock_t *GetSock();                           // get a sock from pool
void ReleaseSock(SlaveSock_t *);                  // release a sock to pool

void SDFiles(int ,char*,char*);                   // send a file to slave
void ListFile(char*,char*);                       // list files in current path
                                                  // and store it into buffer

void scanchar(char *, char ,int *,int);           // a function to capture ch
void *CommunicateThread(void *);                  // a thread to serve
//==============================================================================

 int main()
 {
     //variable

     char msg[] = "WELCOME\n";
     char tmp[1024];
     int client_id;
     int rc;
     
     struct sockaddr_in SockAddr;
     socklen_t AddrLen = sizeof(SockAddr);
	 memset(&SockAddr,0,sizeof(SockAddr));
     
     //initiallize
     MakeSlavePool(AVAILCONNECTION);
     AcceptSock = NULL;
	 ConnectedCount = 0;
     sem_init(&GetSock_Semaphore,0,AVAILCONNECTION);
                               
	 sem_init(&SockCount_Semaphore,0,1);
		                            
     /*
     * multithread for listening & mutex for handle synchronization
     */
     
     MasterSock = MakePassiveSock(HTTP_PORT);
     if(MasterSock<0)
     {
         printf("MasterSock error!\n");                
         exit(-1);
     }
     printf("HTTPP Server Multithread\n");
     
     client_id = 0;
     while(1)
     {
		 sem_wait(&GetSock_Semaphore);
		 sem_wait(&SockCount_Semaphore);
		 ConnectedCount++;
		 sem_post(&SockCount_Semaphore);
         AcceptSock = GetSock();
        
         printf("Thread<%d>: listening....\n",client_id);
		 AcceptSock->skt = accept(
                                  MasterSock,
                                  (struct sockaddr*)&SockAddr,
                                  (socklen_t *)&AddrLen);
         if(AcceptSock->skt < 0)
         {
             printf("accept error! return code:%d\n",AcceptSock->skt);
         }
         rc = pthread_create(
                             &aThread[client_id],
                             NULL,
                             CommunicateThread,
                             (void*)AcceptSock);
	 if(rc)
         {
             printf("pthread_create error! return code : %d\n",rc);    
             exit(-1);
         }    
		 if(client_id<AVAILCONNECTION)client_id++;
		 else client_id = 0;
     }
     printf("End!\n");
     return 0;    
 }

//===========================function impletatiom===============================
int MakePassiveSock(unsigned short int portnumber)
{
    int PassiveSock;
    struct sockaddr_in ServAddr;
    int TransportType;
    struct sctp_initmsg initmsg;
    
    //setting
    memset(&ServAddr , 0 , sizeof(ServAddr));
    ServAddr.sin_family = PF_INET;
    ServAddr.sin_addr.s_addr = INADDR_ANY;
    ServAddr.sin_port = htons(portnumber);
    TransportType = SOCK_STREAM;                                  
    
    //open a socket
    PassiveSock = socket(PF_INET, TransportType, IPPROTO_SCTP);
    if(PassiveSock <0)
    {
        printf("Error to open socket!!!\n");
        exit(-1);
    }
    
    if(bind(PassiveSock,(struct sockaddr*)&ServAddr, sizeof(ServAddr))<0)
    {
        printf("Error to bind socket!!!\n");
        exit(-1);
    }
    
    bzero(&initmsg,sizeof(initmsg));
    initmsg.sinit_num_ostreams = MAXFILETRANSFER;
    initmsg.sinit_max_instreams = MAXFILETRANSFER;
    initmsg.sinit_max_attempts = MAXFILETRANSFER -1;
    setsockopt(PassiveSock,IPPROTO_SCTP,SCTP_INITMSG,&initmsg,sizeof(initmsg));
    
    if(listen(PassiveSock,AVAILCONNECTION)<0)
    {
        printf("Error to listen!!!\n");
        exit(-1);
    }
    return PassiveSock;
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

void MakeSlavePool(int n)
{
     int i;
     SlaveSock_t *mv;
     
     //make one SlaveSock_t first
     AvailSock = (SlaveSock_t*)malloc(sizeof(SlaveSock_t));
     AvailSock->link = NULL;
     mv = AvailSock;
	 mv->id = 0;
     
     //make the rest SlaveSock_t
     for(i=1 ; i<n ; i++)
     {
         mv->link = (SlaveSock_t*)malloc(sizeof(SlaveSock_t));
         mv = mv->link;
         mv->link = NULL;
		 mv->id = i;
     }
}

SlaveSock_t *GetSock()
{
    SlaveSock_t *sock = AvailSock;
	if(AvailSock != NULL)
	{
		AvailSock = AvailSock->link;
		sock->link = NULL;
	}
    return sock;
}

void ReleaseSock(SlaveSock_t *slvskt)
{
    slvskt->link = AvailSock;
    AvailSock = slvskt;     
}


void SDFiles(int skt,char *filelist,char *path)
{
    char sdbuffer[FILEBUFFERSIZE];             //buffer size = 1024 bytes = 1 KB
    char end[7] = "#over#";
    char ch,st_ch,end_ch;
    char filename[100];
    char *finish_send;
    int ReadByte = 0;
    int pointer=0;
    int count_finish_files;
    int ret;
    int i,spacebar_count=0;
    FILE* sdfile_list[MAXFILETRANSFER] ;

    // count the files
    for(i = 0 ; i < strlen(filelist) ; i++)
    {
        ch = filelist[i];
        if(ch == ' ' && filelist[i+1]!=' ')spacebar_count++;
    }
    spacebar_count++;
    
    finish_send = (char*)malloc(sizeof(char)*spacebar_count);

    printf("filelist:<%s>\n",filelist);
    // extract the file names and open files to write
    for(i=0;i<spacebar_count;i++)
    {
        st_ch = pointer;
        scanchar(filelist,' ',&pointer,strlen(filelist));
        end_ch = pointer++;
        if(end_ch == strlen(filelist)) end_ch++;
        bzero(filename,sizeof(filename));
        strcpy(filename,path);
        strcat(filename,"/");
        strncat(filename,filelist+st_ch,end_ch-st_ch);
        sdfile_list[i] = fopen(filename,"rb");
        if(sdfile_list[i] == NULL)
        {
            printf("File open error!!\n");
            send(skt,end,strlen(end),0);
            return ;
        }
        finish_send[i] = '0';
    }

    // read files and send to client
    printf("Client:<#%d> sending file(s) <%s>....\n",skt,filelist);
    while(1)
    {
        for(i = 0 ; i<spacebar_count ; i++)
        {
            ReadByte = fread(sdbuffer,sizeof(char),FILEBUFFERSIZE,sdfile_list[i]);
            if(ReadByte > 0)
            {
                ret = sctp_sendmsg(skt,(void*)sdbuffer,(size_t)ReadByte,NULL,0,0,0,i,0,0);
            }
            //printf("(%d)read:%d byte(s)\n",i,ret);
            memset(sdbuffer,0,FILEBUFFERSIZE);
            if(feof(sdfile_list[i]) != 0 )finish_send[i]='1';
        }
        for(i=0,count_finish_files=0;i<spacebar_count;i++)
        {
            if(finish_send[i] == '1')count_finish_files++;
            //printf("%c ",finish_send[i]);
        }
        //printf("\n");
        if(count_finish_files == spacebar_count)break;
    }
    for(i=0;i<spacebar_count;i++)
    fclose(sdfile_list[i]);
    send(skt,end,sizeof(end),0);
    printf("Client:<#%d> Finish!! <%s>\n",skt,filelist);
    free(finish_send);
    free(sdfile_list);
}

void ListFile(char *filesbuffer, char *currpath)
{
	char cmd[1024];
	char file[100];
	FILE *listfile  ;
	memset(cmd,0,1024);
        memset(filesbuffer,0,sizeof(filesbuffer));
	strcat(cmd,"ls -al ");
	strcat(cmd,currpath);
	listfile = popen(cmd,"r");
	
	if(listfile == NULL)printf("popen error!\n");
        while(1)
	{
            bzero(&file,sizeof(file));
            fgets(file,sizeof(file),listfile);
            if(feof(listfile) != 0)break;
       	    strcat(filesbuffer , file );		
	}
	pclose(listfile);
}

void *CommunicateThread(void *slvskt)
{
     char recvbuf[1024];                        // for sd/recv command
     char sdbuf[1024];                          // for sd/recv msg
     char sdfilename[1024];                     // sending file storage
     char filelist[1024];                       // files in current dir
     char currpath[1024];                       // current path
     char buffer[1024];                         // a buffer for cd

     memset(recvbuf,0,sizeof(recvbuf));
     memset(sdbuf,0,sizeof(sdbuf));
     memset(sdfilename,0,sizeof(sdfilename));
     memset(filelist,0,sizeof(filelist));
     memset(currpath,0,sizeof(currpath));
     memset(buffer,0,sizeof(buffer));
     strcpy(currpath,".");
     printf("inside:<%p>skt:%d\n",&(((SlaveSock_t*)(slvskt))->skt),((SlaveSock_t*)(slvskt))->skt);
     
     printf("Client:<#%d> login!!\n",((SlaveSock_t *)slvskt)->skt);
     printf("Availiable accept:%d\n",AVAILCONNECTION - ConnectedCount + 1);
     memset(sdbuf,0,sizeof(sdbuf));
     //printf("msg size:%d\n",sizeof(msg));
     sprintf(sdbuf,"Welcome<your id:%d>!!\n",((SlaveSock_t *)slvskt)->skt);
     
     if(SendMsg(((SlaveSock_t *)slvskt)->skt,sdbuf)<0)
     {
         printf("SendMsg Error\n");
         exit(-1);
     }
     while(1)
     {
         SendMsg(((SlaveSock_t *)slvskt)->skt,"cmd> ");
         if(RecvMsg(((SlaveSock_t *)slvskt)->skt,recvbuf) <0)break;
         printf("cmd> %s\n",recvbuf);
         if(recvbuf[0] != '#')
         {
             strcpy(sdbuf,"Command starting char must be '#'");
             SendMsg(((SlaveSock_t *)slvskt)->skt,sdbuf);
         }
         else if(strncmp(recvbuf,"#getfiles",strlen("#getfiles")) == 0)
         {
             memset(sdfilename,0,1024);
             strcpy(sdfilename,recvbuf + strlen("#getfiles "));  //extract the file name
             printf("Client:<#%d>: require file <%s>\n",((SlaveSock_t *)slvskt)->skt,sdfilename);
             SDFiles(((SlaveSock_t *)slvskt)->skt,sdfilename,currpath);
         }
         else if(strncmp(recvbuf,"#list",strlen("#list")) == 0)
         {
             memset(filelist,0,sizeof(filelist));
             ListFile(filelist,currpath);
             SendMsg(((SlaveSock_t *)slvskt)->skt,filelist);
         }
         else if(strncmp(recvbuf,"#cd",strlen("#cd")) == 0)
         {
             memset(buffer,0,sizeof(buffer));
             strcpy(buffer,recvbuf + strlen("#cd "));
             if(strcmp(buffer,"..") != 0 && strlen(buffer)>=0)
             {
                 strcat(currpath,"/");
                 strcat(currpath,buffer);
                 printf("<%s>\n",currpath);
             }
             else if(strcmp(currpath,".") != 0 && strcmp(buffer,"..") == 0
                     && strlen(buffer)>=0)
             {
                 *(strrchr(currpath,'/')) = '\0';
             }
             else if(strcmp(currpath,".") == 0 && strcmp(buffer,"..") == 0
                     && strlen(buffer)>=0)
             {
                 //need to do nothing because the top directory is "."
             }
             memset(buffer,0,sizeof(buffer));
             ListFile(buffer,currpath);
             SendMsg(((SlaveSock_t *)slvskt)->skt,buffer);
         }
         else if(strcmp(recvbuf,"#exit") == 0)
         {
            break;                                //terminate the communication
         }
         else
         {
            memset(sdbuf,0,1024);
            strcat(sdbuf,"#list\t\t\tlist server current directory\n");
            strcat(sdbuf,"#cd\t\t\tchange directory - #cd [target directory]\n");
            strcat(sdbuf,"#getfiles\t\tget a file from server - #getfiles [filename..]\n");
            strcat(sdbuf,"#help\t\t\tlist availiable command\n");
            strcat(sdbuf,"#exit\t\t\tterminate the ftp client\n");
            SendMsg(((SlaveSock_t *)slvskt)->skt,sdbuf);
         }
     }
     
     printf("Client:<#%d> logout!!\n",((SlaveSock_t *)slvskt)->skt);	 
     sem_wait(&SockCount_Semaphore);
     ConnectedCount--;
     sem_post(&SockCount_Semaphore);
     printf("Availiable accept:%d\n",AVAILCONNECTION - ConnectedCount + 1);
     sem_post(&GetSock_Semaphore);
     ReleaseSock((SlaveSock_t *)slvskt);
     close(((SlaveSock_t *)slvskt)->skt);
     pthread_exit(NULL);
}
//==============================================================================

void scanchar(char *buffer, char ch,int *pointer,int len)
{
    for(*pointer ; *pointer < len; (*pointer)++)
    {
        if(buffer[*pointer]==ch)break;
    }
}
