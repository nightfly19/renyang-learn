#ifndef _STREAM_COMMON_H_
  #define _STREAM_COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netinet/sctp.h>


//CLIENT AND SERVER
  #define TRANSFER_UNIT unsigned char
  #define MTU_DIFF 2 //transfer unit that protocol header use
  #define MAX_NUM_PARTS 16347 //pow(2,7+7)-1 = mtu_diff unsigned chars
  #define PORT 2014
  #define MTU 12400 //Bytes
//  #define FILE_MTU 104857600//100MB
  #define FILE_MTU 10485760//10MB
  #define FILE_EXT_LEN 10 

  #define END      26
  #define CONTINUE 27
  #define EXIT     28
  #define JOIN     29

  //file information to transfer between client and server
  typedef struct{
      int          mtu; //maximun transfer unit by the network
      int          parts; //parts to split the file to send (depend mtu)
      unsigned int size; //size of the file
      char*        name; //name of the file
      unsigned int name_len; //name file lenght
  } File_inf;

  //parameters to transfer to the thread function in the client
  typedef struct{
      int                     sockfd; //socket assigned to the thread
      TRANSFER_UNIT*          buffer; //buffer to use in the transmision by this thread
      int                     mtu; //maximun transfer unit sent by the network
      int                     num_msgs; //number of parts that have to be sent by the thread
      FILE*                   pFile; //pointer to the file
      int                     parts; //total number of parts which file has been splited
      int                     total; //total number of bytes sent
      struct sctp_sndrcvinfo  sinfo;
  } Thread_inf;

int sendFile(char* ip_addr, int max_channelsIn, int max_channelsOut, char* fileNameOriginal, int several_parts);
int receiveFile(int max_channelsIn, int max_channelsOut);
void usage(int msg);
void printThreadInf(Thread_inf *tinfo);
int  fileExist (char* fileName);
int  mergeFiles(char * fileName, int parts);
int  fsize(FILE *P_File, int mtu, unsigned int *fileSize);
int  fileSplit(char*** pFileNames, char* srcFileName, int file_mtu);
void sendStreams(void *thread_inf);
int  fileReceive(int client_sockfd, int maxChannels);
int  fileSend(int server_sockfd, int maxChannels, char* srcFileName, int mtu);
int  createSocketReceive(int *client_sockfd, int port, int *maxInstreams, int *numOstreams);
int  createSocketSend(int *sockfd, int port, char* addr, int *maxInstreams, int *numOstreams);
void setPos(unsigned char* left, unsigned char* right, int source);
int  getPos(unsigned char left, unsigned char right);

#endif
