// 這一個例子是用來表示如何傳送char的指標到副函式
// 並且透過reference來更改main的資料
#include <iostream>
using namespace std;

void foo(char* a) {
	// 自動會在後面加一個結尾的字元
	strcpy(a,"good");
}

int main(int argc,char **argv) {
	
	char buf[100];
	foo(buf);
	printf("%s\n",buf);
	return 0;
}
