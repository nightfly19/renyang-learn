#include <stdio.h>

int main()
{
	#ifdef MARS
		printf("Hello Mars!\n");
	#else
		printf("Hello World!\n");
	#endif
		return 0;
}
