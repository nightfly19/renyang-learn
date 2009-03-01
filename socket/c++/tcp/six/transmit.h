#ifndef TRANSMIT_H
#define TRANSMIT_H

#include "ServerSocket.h"
#include "ClientSocket.h"
#include "imagedata.h"

void SDFile(ServerSocket *server,char *filename);

void SDStruct(ServerSocket *server,char *pixel);

void RecvFile(ClientSocket *client_socket,char *filename);

void RecvStruct(ClientSocket *client_socket,char *pixel);

#endif
