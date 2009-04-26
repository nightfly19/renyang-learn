#include <stdio.h>

int main(int argc,char **argv)
{
	// 如下宣告, 則只有a是指標變數, 而b是一般的變數
	int* a,b;

	// 只有如下這兩宣告, 才是兩個都是指標
	int *val,*x;

	return 0;
}
