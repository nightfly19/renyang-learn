#include "myevent.h"

MyEvent::MyEvent(int y):QCustomEvent(346798),x(y){
	// do nothing
}

int MyEvent::value() const{
	return x;
}

