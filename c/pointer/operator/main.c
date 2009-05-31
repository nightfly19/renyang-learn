#include <stdio.h>

int main(int argc,char **argv)
{
	char *dataPrt;
	printf("%x\n",dataPrt);
	printf("%x\n",dataPrt-1);
	printf("%x\n",(unsigned short int *) dataPrt-1);
	// 這裡要注意一下, 後面的括號是先用char *作運算, 再轉成(unsigned short int *), 所以, 位址只有減少1 byte
	printf("%x\n",(unsigned short int *) (dataPrt-1));
	return 0;
}
