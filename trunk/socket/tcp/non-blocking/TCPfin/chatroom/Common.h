/******************************************/
/*	Utility for Unix socket programming.
/******************************************/

#ifndef _COMMON_H
#define _COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "setcolor.h"

#define MAX_SERVER_NAME_SIZE 20
#define BUFF_SIZE 2048
char ip[MAX_SERVER_NAME_SIZE],choice[20];

void itoa(int value, char* target);
void chat(int socket);

#endif
