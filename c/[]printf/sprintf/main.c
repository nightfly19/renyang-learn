#include <stdio.h>

int main(){
	int a=100;
	char b='x';
	char c[]="abcd";
	char buf[100];
	sprintf(buf,"%3d%c%s",a,b,c);

	printf("your input is %s\n",buf); // buf是轉換後的暫存區

	return 0;
}
