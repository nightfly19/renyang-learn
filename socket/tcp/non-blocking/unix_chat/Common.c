#include "Common.h"

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
        int i, ret, maxfd = -1; 
        struct timeval tv; 
        fd_set rfds;
        fd_set rfds;
        char buffer[BUFF_SIZE];

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
                ret = select(maxfd + 1, &rfds, NULL, NULL, &tv);

                if(ret == -1){
                        printf("Select() Error: %s", strerror(errno));
                        break;
                }else if(ret == 0){
                        continue;
                }else{

                        /*** check local side's standard input message ***/
                        if(FD_ISSET(0, &rfds)){
                                bzero(buffer, BUFF_SIZE + 1);
                                fgets(buffer, BUFF_SIZE, stdin);

                                if (!strncmp(buffer, ":quit", 5)){
                                        break;
                                }

                                ret = send(socket, buffer, strlen(buffer) - 1, 0);
                                if(ret > 0){
                                        setColor(RED);
                                        printf("\tMe :%s\n",buffer);
                                        setColor(WHITE);
                                }else{
                                        printf("Fail to send message¡I\nReturn Code:%d\nError message:'%s'\n", errno, strerror(errno));
                                        break;
                                }
                        }

                        /*** check socket side's message ***/
                        if(FD_ISSET(socket, &rfds)){
                                bzero(buffer, BUFF_SIZE + 1);
                                ret = recv(socket, buffer, BUFF_SIZE, 0);
                                if(ret > 0){
                                        setColor(BLUE);
                                        printf("\tYou:%s\n",buffer);
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
