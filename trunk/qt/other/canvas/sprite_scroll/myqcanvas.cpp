
#include "myqcanvas.h"

#ifdef MYDEBUG

#include <stdio.h>

#endif

myQCanvas::myQCanvas(int x,int y):QCanvas(x,y){
	
}

void myQCanvas::advance(){
	QCanvas::advance();
#ifdef MYDEBUG
	printf("advance\n");
#endif
	emit move_center_signals();
}
