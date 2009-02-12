#include <stdio.h>

int main(int argc,char *argv[])
{
	int i;
	for (i=0;i<10;i++)
	{
		printf("%d",i);
		// fflush(stdout);	// 把註解刪掉看看
		sleep(1);
	}

	return 0;
}
