#ifndef TRANSMIT_H
#define TRANSMIT_H

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "ServerSocket.h"
#include "ClientSocket.h"
#include "imagedata.h"

void SDFile(ServerSocket *server,char *filename);

void SDStruct(ServerSocket *server,char *pixel);

void RecvFile(ClientSocket *client_socket,char *filename);

void RecvStruct(ClientSocket *client_socket,char *pixel);

// 把IplImage轉換成struct imagedata
void Image2imagedata(IplImage *image,struct imagedata &data);

#endif
