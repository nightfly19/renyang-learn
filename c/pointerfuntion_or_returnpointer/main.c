#include <stdio.h>
#include <stdlib.h>

int* getvalue()
{
	int *y;
	y = malloc(sizeof(int));
	*y=1;
	return y;
}

int main()
{
	int *x;
	x = getvalue();
	printf("value:%d\n",*x);

	return 0;
}
