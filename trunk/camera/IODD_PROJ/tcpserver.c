#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include "cv.h"
#include <highgui.h>

	//int socket(int domain, int type, int protocol);

static int
ColorInterpolate (unsigned char  *pOutputBuffer, unsigned char *pImageBuffer, int nImageWidth, int nImageHeight);


int main(int argc,char *argv[])
{
	int server_sockfd,client_sockfd;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;
	int server_len,client_len;

	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr=inet_addr("192.168.0.127");
	server_address.sin_port = 20000;
	server_address.sin_port = htons(20000);

	unsigned char *pImageBuffer=NULL;
	unsigned char *pImageBufferCopy=NULL;
	unsigned char *pRGBBuffer=NULL;
	unsigned char *bufTmp=NULL;
	int imagesize=320*240;
	int tmp;

	int sum;
	int i;
	char ch;
	int ret;
	char temp[10];
	char init1[] = {0x00,0x01,0x1c,0x00,0xe0,0xf5,0x78,0x00};
	char init2[] = {0x02,0xfd,0x40,0x01,0xf0,0x00,0x78,0x74};
	int c=0;
	IplImage* co;
	
	server_len = sizeof(server_address);
	bind(server_sockfd,(struct sockaddr*) & server_address, server_len);
	listen(server_sockfd,5);

	printf("server waiting \n");
	client_len = sizeof(client_address);
	client_sockfd = accept(server_sockfd,(struct sockaddr *)&client_address, &client_len);

	write(client_sockfd,&init1,sizeof(init1));

	pImageBuffer = malloc(320*240+1392);
	pImageBufferCopy = pImageBuffer;
	pRGBBuffer = malloc(320*240*3);

	
	co=cvCreateImage(cvSize(320,240),IPL_DEPTH_8U,3);

	while(1)
	{
		pImageBuffer = pImageBufferCopy;
		tmp = imagesize;
		memset(pImageBuffer,0,320*240);
		
		write(client_sockfd,&init2,sizeof(init2));

		//read(client_sockfd, &temp, 4);
		
		ret = 0;

		while(tmp > 0) {
			bufTmp = malloc(1448);
			ret = read(client_sockfd, bufTmp,1448 );
			
			if(ret == 3) {
			    printf("Bye Bye\n");
			    return 0;
			}
			
			memcpy(pImageBuffer, bufTmp, 1448);
			pImageBuffer+=1448;
			tmp -= ret;
			//printf("%d \n",ret );
			c++;
		}


		/*FILE *fd;
		fd = fopen("raw.dat","w");
		for(i=0;i<320*240;i++)
			fprintf(fd,"%d ",pImageBufferCopy[i]);

		fclose(fd);
printf("ass hole3\n");*/
		//printf("%s\n", pImageBuffer);
		ColorInterpolate(pRGBBuffer,pImageBufferCopy,320,240);

		printf("%d\n", c);
		
		
		int height	= 240;
		int width	= 320;
		int channels=3;
		//int i;
		uchar *data	=(uchar *)co->imageData;
		
		for(i=0;i<height*width;i++)
		{
				data[i*3+2]=pRGBBuffer[i*3];
				data[i*3+1]=pRGBBuffer[i*3+1];
				data[i*3]=pRGBBuffer[i*3+2];
				//printf("%d ", data[i]);
				
		}		

//printf("ass hole1\n");									
		
		cvNamedWindow ("Image", 100);
	  	cvShowImage ("Image", co);
		cvWaitKey(10);
//printf("ass hole2\n");	
		//cvReleaseCapture(&co);
//printf("ass hole3\n");	
	}
	
	cvDestroyWindow( "Image" );
	
	close(client_sockfd);
	return 0;
}

int
ColorInterpolate (unsigned char *pOutputBuffer, unsigned char *pImageBuffer, int nImageWidth, int nImageHeight)
{
        int              i, j;
        int 	         ph ,pw;
        unsigned long    pre_Offset, p_Offset, post_Offset;
        unsigned int     Rval, Gval, Bval ;
        unsigned char    byIRemainder, byJRemainder;
        unsigned char    R, G, B, Counts;
        unsigned char    *cp,  data, *pOutputRGB, *p, *cast;

        ph = nImageHeight; pw = nImageWidth;
        cp = pImageBuffer; pOutputRGB = pOutputBuffer;
        // Color
        
        for(i=1; i < ph; i++){
            pre_Offset = (i-1)*pw; p_Offset=(i*pw), post_Offset = (i+1)*pw;
            byIRemainder = i & 1;
            for (j=0; j < pw; j++){
                Rval = Gval = Bval = 0;
                 byJRemainder = j & 1;

                // At Ri,j --> i%2 == 0 && j%2 == 0
                if (byIRemainder == 0 &&  byJRemainder == 0){
                    // R = Ri,j
                    // G = (Gri,j-1  + Gri,j+1  + Gbi-1,j  + Gbi+1,j) /4
                    // B = (Bi-1,j-1 + Bi-1,j+1 + Bi+1,j-1 + Bi+1,j+1)/4
                    R = *cp;

                    // Get G value
                    Counts = 0;
                    if ((j - 1) >= 0){
                        Gval += pImageBuffer[(p_Offset) +(j-1)];
                        Counts++;
                    }
                    if ((j+1) < pw){
                        Gval += pImageBuffer[(p_Offset) +(j+1)];
                        Counts++;
                    }
                    if ((i - 1) >= 0){
                        Gval += pImageBuffer[(pre_Offset) +j];
                        Counts++;
                    }
                    if ((i+1) < ph){
                        Gval += pImageBuffer[(post_Offset) +j];
                        Counts++;
                    }
                    G = (unsigned char)(Gval / Counts);

                    //Get B
                    Counts = 0;
                    if ((i-1) >= 0 && (j-1) >= 0){
                        Bval += pImageBuffer[(pre_Offset) +(j-1)];
                        Counts++;
                    }
                    if ((i-1) >= 0 && (j+1) < pw){
                        Bval += pImageBuffer[(pre_Offset) +(j+1)];
                        Counts++;
                    }
                    if ((i+1) < ph && (j-1) >= 0){
                        Bval += pImageBuffer[(post_Offset) +(j-1)];
                        Counts++;
                    }
                    if ((i+1) < ph && (j+1) < pw){
                        Bval += pImageBuffer[(post_Offset) +(j+1)];
                        Counts++;
                    }
                    B = (unsigned char)(Bval / Counts);

                }
                //At Gri,j --> i%2 == 0 && j%2 == 1
                else if (byIRemainder == 0 &&  byJRemainder == 1){
                    // R = (Ri,j-1 + Ri,j+1) /2
                    // G = Gri,j
                    // B = (Bi-1,j + Bi+1,j) /2

                    // Get R
                    Counts = 0;
                    if ((j-1) >= 0){
                        Rval +=  pImageBuffer[(p_Offset)+(j-1)];
                        Counts++;
                    }
                    if ((j+1) < pw){
                        Rval += pImageBuffer[(p_Offset)+(j+1)];
                        Counts++;
                    }
                    R = (unsigned char)(Rval / Counts);

                    // Get G
                    G = *cp;

                    // Get B
                    Counts = 0;
                    if ((i-1) >= 0){
                        Bval += pImageBuffer[(pre_Offset)+(j)];
                        Counts++;
                    }
                    if ((i+1) < ph){
                        Bval += pImageBuffer[(post_Offset)+(j)];
                        Counts++;
                    }
                    B = (unsigned char)(Bval / Counts);
                }
                //At Gbi,j --> i%2 == 1 && j%2 == 0
                else if (byIRemainder == 1 &&  byJRemainder == 0){
                    // R = (Ri-1,j + Ri+1,j) /2
                    // G = Gbi,j
                    // B = (Bi,j-1 + Bi,j+1) /2

                    // Get R
                    Counts  = 0;
                    if ((i-1) >= 0){
                        Rval  += pImageBuffer[(pre_Offset)+(j)];
                        Counts++;
                    }
                    if ((i+1) < ph){
                        Rval += pImageBuffer[(post_Offset)+(j)];
                        Counts++;
                    }
                    R = (unsigned char)(Rval / Counts);

                    // Get G
                    G = *cp;

                    // Get B
                    Counts  = 0;
                    if ((j-1) >= 0){
                        Bval  += pImageBuffer[(p_Offset)+(j-1)];
                        Counts++;
                    }
                    if ((j+1) < pw){
                        Bval += pImageBuffer[(p_Offset)+(j+1)];
                        Counts++;
                    }
                    B = (unsigned char)(Bval /Counts);;
                }
                //At Bi,j --> i%2 == 1 && j%2 == 1
                else{
                    // R = (Ri-1,j-1 + Ri-1,j+1 + Ri+1,j-1 + Ri+1,j+1) /4
                    // G = (Gbi,j-1  + Gbi,j+1  + Gri-1,j  + Gri+1,j)  /4
                    // B = Bi,j;

                    // Get R
                    Counts  = 0;
                    if ((i-1) >= 0 && (j-1) >= 0){
                        Rval += pImageBuffer[(pre_Offset) +(j-1)];
                        Counts++;
                    }
                    if ((i-1) >= 0 && (j +1) < pw){
                        Rval += pImageBuffer[(pre_Offset) +(j+1)];
                        Counts++;
                    }
                    if ((i+1) < ph && (j-1) >= 0){
                        Rval += pImageBuffer[(post_Offset) +(j-1)];
                        Counts++;
                    }
                    if ((i+1) < ph && (j+1) < pw){
                        Rval += pImageBuffer[(post_Offset) +(j+1)];
                        Counts++;
                    }
                    R = (unsigned char)(Rval /Counts);

                    //Get G
                    Counts  = 0;
                    if ((j-1) >= 0){
                        Gval += pImageBuffer[(p_Offset) +(j-1)];
                        Counts++;
                    }
                    if ((j+1) < pw){
                        Gval += pImageBuffer[(p_Offset) +(j+1)];
                        Counts++;
                    }
                    if ((i-1) >= 0){
                        Gval += pImageBuffer[(pre_Offset) +j];
                        Counts++;
                    }
                    if ((i+1) < ph){
                        Gval += pImageBuffer[(post_Offset) +j];
                        Counts++;
                    }
                    G = (unsigned char)(Gval / Counts);;

                    // Get B
                    B = *cp;
                }
                cp++;

                *pOutputRGB++ = R;
                *pOutputRGB++ = G;
                *pOutputRGB++ = B;                
            }//pw
        }//ph     

        //compensate line 0
	    p =  pOutputBuffer + (nImageWidth *3);  // line 1
	    cast = pOutputBuffer;                   // line 0
	    for (j=0; j < nImageWidth; j++)	{
	    	*cast++=(*p++);	*cast++=(*p++);	*cast++=(*p++);
    	}
    	
    	//compensate line (nImageHeight-1)
	    p =  pOutputBuffer + ((nImageHeight - 2)* nImageWidth *3);   // line Height-2 	    
	    cast = pOutputBuffer + ((nImageHeight - 1)* nImageWidth *3); // line Height-1   
	    for (j=0; j< nImageWidth; j++){	
	    	*cast++=(*p++);	*cast++=(*p++);	*cast++=(*p++);
        }    	
}

