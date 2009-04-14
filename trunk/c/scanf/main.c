#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	char input[100];
	printf("please in a string having space between world:");
	// fgets(input,100,stdin);
	gets(input);
	printf("you input is:\n");
	printf("%s",input);
	printf("%d\n",strlen(input));
	return 0;
}
