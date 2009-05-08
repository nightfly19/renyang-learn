#include "classB.h"

classB::classB()
{
	x = new int(3);
}

void classB::Slotvalue(int *y)
{
	printf("%d\n",*y);
}
