#include <stdio.h>

int main(int argc,char **argv)
{
	char *dataPrt;
	printf("%x\n",dataPrt);
	printf("%x\n",dataPrt-1);
	printf("%x\n",(unsigned short int *) dataPrt-1);
	return 0;
}
