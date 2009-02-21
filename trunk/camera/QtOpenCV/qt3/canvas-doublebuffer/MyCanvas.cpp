
#include "MyCanvas.h"

MyCanvas::MyCanvas(int w,int h):QCanvas(w,h) {
	// setDoubleBuffering(true);
	setAdvancePeriod(50);
}
