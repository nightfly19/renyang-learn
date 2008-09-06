
#include <iostream>

using namespace std;

#include "food.h"

int main(int argc,char *argv[])
{
	MikanBox myMikanBox;

	for (int loop_index=0;loop_index<argc;loop_index++)
	{
		cout << argv[loop_index] << endl;
	}

	return 0;
}
