#include <stdio.h>
#include <stdlib.h>

int main()
{
	int *a = malloc(5*sizeof(int));
	int i;

	for (i=0;i<5;i++)
	{
		a[i]=i;
	}

	for (i=0;i<5;i++)
	{
		printf("%d\n",a[i]);
	}

	return 0;
}
