#include "classA.h"

classA::classA()
{
	x = new int(9);
}

void classA::sendsignal()
{
	emit Signalvalue(x);
}
