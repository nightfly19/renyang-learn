
#include <iostream>
#include <time.h>

using namespace std;

const int MAX=100;

int main(int argc,char *argv[])
{
	srand((unsigned)time(NULL));
	for (int i=0;i<10;i++)
		cout << rand()%MAX<<endl;
	return 0;
}
