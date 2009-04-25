#include <stdio.h>
#include <signal.h>

void exithandle(int sig)
{
	printf("signal id=%d received\n",sig);
}

int main(int argc,char **argv)
{
	// 註冊使得當此程式收到INT(ctrl+c)的訊號時, 會執行exithandle的函式
	signal(SIGINT,exithandle);
	pause();
	printf("I want to leave\n");
	return 0;
}
