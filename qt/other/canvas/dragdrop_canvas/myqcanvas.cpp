
#include "myqcanvas.h"

#ifdef MYDEBUG

  #include <iostream>
  using namespace std;

#endif

MyQCanvas::MyQCanvas(int w,int h):QCanvas(w,h)
{
#ifdef MYDEBUG
	cout << "debug is open" << endl;
#endif
}

void MyQCanvas::advance()
{
	QCanvas::advance();
}

