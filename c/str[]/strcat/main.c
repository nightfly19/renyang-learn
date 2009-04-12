#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	char buf[100];
	strcpy(buf,"Hello World");
	printf("%s",buf);
	strcat(buf,"!!");
	printf("%s",buf);
	/*
	以下是錯誤的例子,因為會超出所宣告的記憶體範圍
	char buf[] = "Hello World!!";
	strcat(buf,"!!");
	 */
}
