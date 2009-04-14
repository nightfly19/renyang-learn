#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	FILE *fp;
	char filename[]="main.c";
	fp = fopen(filename,"rb");
	if (fp == NULL)
		printf("open error:%s\n",filename);
	else {
		printf("open success:%s\n",filename);
		fclose(fp);
	}
	
	return 0;
}
