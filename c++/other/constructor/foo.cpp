
#include <stdio.h>
#include <stdlib.h>

#include "foo.h"

foo::foo()
{
	printf("Hello\n");
}

foo::foo(int x)
{
	printf("Hello:%d\n",x);
}
