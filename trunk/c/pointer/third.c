#include <stdio.h>
#include <stdlib.h>

int main()
{
	// 表示宣告一個陣列，可以存放5個int *的空間
	int *a[5];
	int i;
	for(i=0;i<5;i++)
	{
		a[i] = malloc(sizeof(int));
		*a[i] = i;
	}
	for (i=0;i<5;i++)
	{
		printf("%d\n",*a[i]);
	}

	return 0;
}
