#include "Common.h"
#include <time.h>

void itoa(int value, char* target) 
{
	int i=0, index;
	int isRmZeros = 1;
	unsigned char ch;
	
	/*** To deal with negative value ***/
	if(value<0){
 		target[i++]='-';
 		value*=-1;
 	}
	
	/*** For special case for 0 value ***/
	if(value==0){
		target[i++]='0';
		target[i]='\0';
		return;
	}
	
	/*** We set index = 1000000000 for 4 bytes integer ****/
	for(index=1000000000; index>=1; index/=10){
		ch=value/index+'0';
		if(ch=='0'&&isRmZeros) continue;
		else{
			isRmZeros=0;
			target[i++]=ch;
			value%=index;
		}
	}
	target[i]='\0';
}

void chat(int socket)
{
	//pthread_t transfer_thread,receive_thread;
	int i, ret, maxfd = -1;
	char *p;
	struct timeval tv;
	fd_set rfds;
	FILE *fp;
	char buffer[BUFF_SIZE],bufferT[BUFF_SIZE];
	time_t timep;
	
	
	fp = fopen("record","wb");	
	
	if (fp==NULL) {
                        printf("open file error!!\n");
                        exit(1);
                }
	

	printf("\nReady for sending message!\n");
	while (1){
		/*** initiaize FD ****/
		FD_ZERO(&rfds);
		FD_SET(0, &rfds);
		maxfd = 0;
		FD_SET(socket, &rfds);

		if (socket > maxfd){
			maxfd = socket;
		}	
		
		/*** set timeout and wait ***/
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		//printf("befor select\n");
		fflush(stdin);
		ret = select(maxfd + 1, &rfds, NULL, NULL, &tv);
		
		if(ret == -1){
			printf("Select() Error: %s", strerror(errno));
			break;
		}else if(ret == 0){
			continue;
		}else{
			
			/*** check local side's standard input message ***/
			if(FD_ISSET(0, &rfds)){
				//printf("1\n");
				//printf("2\n");
				bzero(buffer, BUFF_SIZE + 1);
				//printf("3\n");
				fflush(stdin);
				fgets(buffer, BUFF_SIZE, stdin);	
				//printf("4\n");
				strcpy(bufferT,buffer);
				//printf("buffer=%s\n",buffer);
//printf("ip=%s\n",ip);
				
				if (!strncmp(buffer, ":quit", 5)){
					fclose(fp);
					break;
				}
				
			   if (!strncmp(bufferT, "transfer", 8)){
					send(socket, buffer, strlen(buffer) - 1, 0);
					p=strtok(bufferT," ");
					p=strtok(NULL," ");
					//printf("transfer %s\n",p);
					transSer(p);
					
			   }	
			   else{
				//printf("********\n");
				//printf("socket=%d\n",socket);
				
				ret = send(socket, buffer, strlen(buffer) - 1, 0);
				if(ret > 0){
				//	setColor(RED);
				
					printf("Local :%s",buffer);
				//	printf("\tlen :%d\n",strlen(buffer));
					time (&timep);
				//	printf("%s\n",asctime(gmtime(&timep)));
					fwrite(asctime(gmtime(&timep)),strlen(asctime(gmtime(&timep))),1,fp);
					fwrite("Local: ",7,1,fp);
					fwrite(buffer,strlen(buffer),1,fp);
				//	fwrite("\n",1,1,fp);

					//fwrite(buffer,strlen(buffer)+1,sizeof(char),fp);
					setColor(WHITE);
				}
				else{
					printf("Fail to send message¡I\nReturn Code:%d\nError message:'%s'\n", errno, strerror(errno));
					break;
				}
			   }


			//printf("testS\n");

			}
			
			/*** check socket side's message ***/
			if(FD_ISSET(socket, &rfds)){
				bzero(buffer, BUFF_SIZE + 1);
				ret = recv(socket, buffer, BUFF_SIZE, 0);
				if(ret > 0){
				//	setColor(BLUE);
					
					printf("Remote:%s\n",buffer);
				//	fwrite(&buffer,sizeof(char),sizeof(buffer),fp); 
					time (&timep);
			//	printf("%s\n",asctime(gmtime(&timep)));
					fwrite(asctime(gmtime(&timep)),strlen(asctime(gmtime(&timep))),1,fp);
					fwrite("  Remote: ",10,1,fp);
					fwrite(buffer,strlen(buffer),1,fp);
					fwrite("\n",1,1,fp);
					if (!strncmp(buffer, "transfer", 8)){
					transCli();
					printf("data accept \n");
					}
				
				setColor(WHITE);
				}else{
					if (ret < 0){
						printf("Fail to receive message¡I\nReturn Code:%d\nError message:'%s'\n", errno, strerror(errno));
					}else{
						setColor(BYELLOW);
						printf("He/She is offline!\n");
						setColor(WHITE);
						break;
					}
				}
			}
			
		}
	}
}
