#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	char str1[100];
	char str2[100];
	bzero(str1,100);
	bzero(str2,100);
	printf("%d\n",strlen("123 567"));
	printf("%d\n",sizeof("123 567"));

	return 0;
}
