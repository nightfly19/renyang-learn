#include <stdio.h>

int main(int argc,char **argv)
{
	char dataPrt[100];
	printf("%x\n",dataPrt);
	printf("%x\n",dataPrt-1);
	printf("%x\n",(unsigned short int *) dataPrt-1);
	// 這裡要注意一下, 後面的括號是先用char *作運算, 再轉成(unsigned short int *), 所以, 位址只有減少1 byte
	printf("%x\n",(unsigned short int *) (dataPrt-1));

	// 以unsigned short int的形式把資料塞到char陣列中
	unsigned short int *x;
	x = (unsigned short int *)dataPrt;
	*x = 1024;
	printf("%d\n",*((unsigned short int *) dataPrt));
	return 0;
}
