#include <stdio.h>
#include <stdlib.h>

int main() {
	char buffer[100];
	printf("Please input string:");
	fread(buffer,1,100,stdin);
	printf("You input is:'%s'",buffer);
	return 0;
}
