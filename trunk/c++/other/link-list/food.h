
#ifndef FOOD_H
#define FOOD_H

class MikanBox
{
	public:
		MikanBox();
		MikanBox(MikanBox*);
	private:
		int total;
		MikanBox* next;
};

#endif // FOOD_H

