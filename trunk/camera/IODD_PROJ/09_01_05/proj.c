#include <signal.h>
#include <stdio.h>
#include <strings.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <malloc.h>
#include "def.h"
#include "creator_pxa270_lcd.h"
#include "creator_pxa270_cmos.h"
#include "creator_lib.h"

#define PORTNUM 	20000
/*************************************************************************
Variable define
*************************************************************************/
lcd_full_image_info_t	ImageInfo;
char hostIP[4] = { 192, 168, 0, 127 };
int		fd, i, ret=OK, terminate ;

int globle_flag;
/*

int
main (void)
{
        
	
     	   fd = open("/dev/lcd", O_RDWR);
     	   if (fd < 0){
            printf("open /dev/lcd error\n");
            return (-1);
	    }

	LCD_fClearScreen(fd);
	LCD_fprintf(fd, "running...\n");
	ProcessImageFromEthernetToPC(fd, FALSE);


globle_flag
        return (0);
}
*/
int
ProcessImageFromEthernetToPC (int* fd_lcd)
{
        cmos_info_t	CMOSInfo;	
        int 		ret =OK, fd_cmos=-1, fd_socket=-1, WaitImage;	
        int 		nRead, nWrite, terminate, CMOSRunning, ImgW, ImgH;		
        unsigned long	dwImageSize, dwTotalReadSize, count ;
        unsigned short	key;
        unsigned char	*pImageBuffer=NULL, *pMsg, *pRGBBuffer=NULL;
//printf("ass hole1\n");
        fd_socket = call_socket(PORTNUM);
/*        if (fd_socket == -1){        
            LCD_ErrorMessage(fd_lcd, "Socket error\nAny to exit:");		    	
            goto out;
        }
*/
//printf("ass hole2\n");
        fd_cmos = open("/dev/cmos", O_RDWR);
   /*     if (fd_cmos < 0) {
            LCD_ErrorMessage(fd_lcd, "Open ccm error\nAny to exit:");	
            return (UM);
        }
*/
        pImageBuffer = malloc(320*240);
        pRGBBuffer = malloc(320*240*3);
 /*       if (pImageBuffer == NULL || pRGBBuffer == NULL){
            LCD_ErrorMessage(fd_lcd, "mem alloc error\nAny to exit:");		
            goto out;	    	    	
        }
*/
        terminate = 1;		
        CMOSRunning = 0;
        pMsg = "\nImage to PC...";
        printf("%s", pMsg);
//       if (!ImageToLCD)
 //           LCD_printf("%s", pMsg);	
        while (terminate){	
		if(globle_flag == 1) 
			goto out;
            /* 等待PC 傳送command */	
            nRead = recv(fd_socket, &CMOSInfo, sizeof(cmos_info_t), 0);
//            if (nRead == 0){
 //	            LCD_ErrorMessage(fd_lcd, "socket break\n\n Any to exit:");		    	
  //              terminate = 0;
      //      	}    
    	    
	//CMOS_INFO cmosinfo;
	/*CMOSInfo.command=PC_CMOS_ON;
	CMOSInfo.ImageSizeMode=CMOS_SIZE_MODE_160_120;
	CMOSInfo.ImageWidth=160;
	CMOSInfo.ImageHeight=120;
	CMOSInfo.HighRef=120;
	CMOSInfo.Status=0;*/

            if (CMOSInfo.command == PC_CMOS_ON){    
                ret = ioctl(fd_cmos, CMOS_ON, &CMOSInfo);		    	
  //              if (ret < 0){
  //                  LCD_ErrorMessage(fd_lcd, "CCM ON error\n\n Any to exit:");		
  //                  goto out;			    
//		        }
                ImgW = CMOSInfo.ImageWidth ; ImgH = CMOSInfo.ImageHeight ;		
                dwImageSize = ImgW * ImgH ;
                continue ;
            }
            else if (CMOSInfo.command == PC_CMOS_OFF){	    
                ioctl(fd_cmos, CMOS_OFF, &CMOSInfo);	
                goto out;
            }		
            ioctl(fd_cmos, CMOS_PARAMETER, &CMOSInfo); 	    
    		
            WaitImage = 1;
            while(WaitImage){ 	
                if (ioctl(fd_cmos, CMOS_GET_STATUS, &CMOSInfo) >= 0){
                    if (CMOSInfo.Status == CMOS_IMG_READY){
                        /* 讀取影像 */	
                        dwTotalReadSize = 0; count = dwImageSize; 		
                        do {	    	
                           if (count + dwTotalReadSize > dwImageSize)
                               count = dwImageSize - dwTotalReadSize ;
	    	
                           nRead = read(fd_cmos, pImageBuffer+dwTotalReadSize, count);	
                           if (nRead > 0 ){
                               dwTotalReadSize += nRead;
                           }
                           else if (nRead == 0){
                               break;	
                           }
 	  	                   else{
 	  	                       //break;
                           }
                        } while (dwTotalReadSize < dwImageSize); 
            		
                        /* 傳送Image raw data to PC */	               
                        nWrite = write(fd_socket, pImageBuffer, dwImageSize);
				
         /*               if (ImageToLCD){
                            ColorInterpolate(pRGBBuffer, pImageBuffer, ImgW, ImgH);
                            DrawRawDataToLCD(fd_lcd, fd_cmos, pRGBBuffer, &CMOSInfo, ImgW, ImgH);	 		
                        }            
          */              WaitImage = 0;       			    
                    }
                }
     //           if (KEYPAD_get_key(fd_lcd, &key) == OK){          	  
    //                goto out;
              //  }		        
                else if (CMOSInfo.Status == CMOS_IMG_EMPTY){
                    /* 等待影像資料*/
                    //Delay(200);			
                }
            }
        }
out:
	  nWrite = write(fd_socket, "abc", 3);
        if (pImageBuffer)
            free(pImageBuffer);
        if (pRGBBuffer)
            free(pRGBBuffer);            
        if (fd_cmos < 0){
            ioctl(fd_cmos, CMOS_OFF, &CMOSInfo);	
            close(fd_cmos);	
        }
        if (fd_socket != -1){
            close(fd_socket);
            printf("socket close\n");
        }
	
        return (OK);	
	
}	


int 
call_socket (int portnum)
{
        struct 	sockaddr_in 	sa;
        int 			        fd_socket;
        unsigned char		    *cp;
	
        bzero(&sa, sizeof(sa));
        sa.sin_family = AF_INET;
        sa.sin_port = htons((u_short)portnum);

        printf("create socket...\n");
        if ((fd_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
            printf("Unable to obtain network\n");			
            return (-1);
        }

        cp = (unsigned char*) &sa.sin_addr;
        cp[0] = hostIP[0]; cp[1] = hostIP[1]; cp[2] = hostIP[2]; cp[3] = hostIP[3];
        printf("connect to %s...", hostIP);	
        if (connect(fd_socket, (struct sockaddr*)&sa, sizeof(sa)) < 0) {
            printf("Unable to obtain network\n");		
            close(fd_socket);
            return (-1);
        }
        printf("OK\n");	
        return (fd_socket);
}


