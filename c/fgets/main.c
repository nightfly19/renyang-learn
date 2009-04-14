/* fgets exmaple */
#include <stdio.h>
#include <string.h>

int main()
{
	char buffer[10];
	printf("Please input a string:");
	fgets(buffer,10,stdin);
	printf("You input is:%s",buffer);
	printf("strlen:%d\n",strlen(buffer));
	return 0;
}
