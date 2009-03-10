#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

struct test
{
	int no;
	char name[80];
};

void *pfunc(void *data)
{
	// 後面的括號是把void型態的指標轉成strut test的指標
	// 而前面的*號是後面指標指到的位址提取出來
	struct test tt = * (struct test*) data;
	printf("%d\n",tt.no);
	printf("%s\n",tt.name);
}

int main()
{
	struct test itest;
	pthread_t th;
	itest.no = 100;
	strcpy(itest.name,"Hello");

	// 產生一個thread，其去執行pfunc這一個函數
	pthread_create(&th,NULL,pfunc,(void*) &itest);
	return 0;
}
