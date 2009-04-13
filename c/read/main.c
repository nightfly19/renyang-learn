#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int main()
{
	char buffer[100];
	int readbyte;
	printf("Please input a string:");
	readbyte = read(0,buffer,100);
	printf("You input is %s",buffer);
	printf("The lengh is %d\n",readbyte);
	return 0;
}
