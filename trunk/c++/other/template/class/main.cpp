
#include <iostream>

template <class T>
class Recangle{
	public:
		T GetRight(){
			return left+width;
		}

		T left,top,width,height;
};

