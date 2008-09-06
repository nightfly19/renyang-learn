
#include "food.h"

MikanBox::MikanBox()
{
	next = 0;
}

MikanBox::MikanBox(MikanBox* add)
{
	next = add;
}

