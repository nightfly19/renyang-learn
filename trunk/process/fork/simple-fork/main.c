#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
	pid_t id;
	int x=0;
			// fork()以上的資源數值會由兩個程序共享
	id = fork();	// 這一行會產生兩個process，並且都會執行以下的程式碼
			// 以下的process均會有x變數,而且兩個值均是獨立的,但初始值均與fork()前的相同
	if (id>0)
	{
		++x;
		printf("I am parent!!%d\n",x);
	}
	else
	{
		++x;
		printf("I am child!!%d\n",x);
	}
	return 0;
}
