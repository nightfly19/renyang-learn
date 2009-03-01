#include "transmit.h"

void SDFile(ServerSocket *server,char *filename)
{
        char sdbuffer[MAXRECV];
        char end[7]="#over#";
        int ReadByte=0;
        FILE *sdfile = fopen(filename,"rb");
        if (sdfile==NULL)
        {
                printf("open %s file error!!\n",filename);
                *server << end;
                return ;
        }
        printf("Client: sending file <%s>",filename);

        while((ReadByte=fread(sdbuffer,sizeof(char),MAXRECV,sdfile))>0)
        {
                printf("fread:%d\n",ReadByte);
                //printf(".");
                server->Sendbyte(sdbuffer,ReadByte);
                memset(sdbuffer,0,MAXRECV);
        }
        fclose(sdfile);
        // 若這邊不sleep一下的話,上面的最後一個片段會與下面的結束片斷一起傳送,就會造成傳送過去的資料判斷錯誤
        // 透過non-blocking就可以避免使用sleep啦
        // sleep(1);
        *server<<end;
        printf("\nClient:<%s> Finish!!\n",filename);
}

void SDStruct(ServerSocket *server,char *pixel)
{
        char sdbuffer[MAXRECV];
        char end[7]="#over#";
        int ReadByte=0;
        int begin=0;
        int structsize=sizeof(struct imagedata);
        do
        {
                if (structsize>=begin+MAXRECV)
                {
                        ReadByte=MAXRECV;
                }
                else
                {
                        ReadByte=structsize-begin;
                }
                if (ReadByte>0)
                {
                        memset(sdbuffer,0,MAXRECV);
                        strncpy(sdbuffer,pixel+begin,ReadByte);
                        server->Sendbyte(sdbuffer,ReadByte);
                        begin+=ReadByte;
                }
                else
                {
                        break;
                }
        }while(true);
        //sleep(1);
        *server<<end;
        printf("\nClient imagedata Finish!!\n");
}

void RecvFile(ClientSocket *client_socket,char *filename)
{
        FILE *fp = fopen(filename,"wb");
        char filebuffer[MAXRECV];
        memset(filebuffer,0,MAXRECV);
        int Readbyte=0;
        Readbyte=*client_socket>>filebuffer;
        while(strncmp(filebuffer,"#over#",strlen("#over#"))!=0)
        {
                fwrite(filebuffer,sizeof(char),Readbyte,fp);
                Readbyte=*client_socket>>filebuffer;
                printf("x");
        }
        fclose(fp);
}

void RecvStruct(ClientSocket *client_socket,char *pixel)
{
        char filebuffer[MAXRECV];
        memset(filebuffer,0,MAXRECV);
        int Readbyte=0;
        int begin=0;
        Readbyte=*client_socket>>filebuffer;
        while(strncmp(filebuffer,"#over#",strlen("#over#"))!=0)
        {
                strncpy(pixel+begin,filebuffer,Readbyte);
                begin+=Readbyte;
                Readbyte=*client_socket>>filebuffer;
                printf("y");
        }
}

void Image2imagedata(IplImage *cvimage,struct imagedata &data)
{
	int cvIndex=0,cvLineStart=0;
	for (int y=0;y<cvimage->height;y++) {
		cvIndex = cvLineStart;
		for (int x=0;x<cvimage->width;x++) {
			data.r_pixel[y][x]=cvimage->imageData[cvIndex+2];
			data.g_pixel[y][x]=cvimage->imageData[cvIndex+1];
			data.b_pixel[y][x]=cvimage->imageData[cvIndex+0];
			cvIndex+=3;
		}
		cvLineStart += cvimage->widthStep;
	}
}
