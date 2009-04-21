/******************************************/
//	Utility for Unix socket programming.
/******************************************/

#ifndef _COMMON_H
#define _COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "setcolor.h"

#define BUFF_SIZE 2048

// renyang - 把整數轉成字串
void itoa(int value, char* target);
void chat(int socket);

#endif
