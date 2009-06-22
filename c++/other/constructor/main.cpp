
#include "foo.h"

#include <stdio.h>
#include <stdlib.h>

int main()
{
	printf("0\n");
	// 若要建立一個無參數的物件, 不可以加括號
	foo x;
	printf("1\n");
	// 若加了括號會沒有效果，我也不知道為啥會這樣
	foo z();
	printf("2\n");
	// 若要建立一個有參數的物件，很明顯的，一定要有括號，且裡面要有數值
	foo y(1);
	printf("3\n");
	// 要產生一個空間放置一個無參數的foo物件, 就可以不需要括號
	foo *a = new foo;
	printf("4\n");
	// 但是，要產生一個空間放置一個無參數的foo的物件，那麼用括號是可以的
	foo *b = new foo();
	printf("5\n");
	// 要產生一 個空間放置一個有參數的foo物件, 很明顯的一定要有括號，且裡面要有數值
	foo *c = new foo(2);
	printf("6\n");
	return 0;
}
