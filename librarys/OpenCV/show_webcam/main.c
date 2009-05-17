#include <cv.h>
#include <highgui.h>
#include <stdio.h>

int main()
{
    CvCapture *capture;
    IplImage *frame;
    capture =cvCaptureFromCAM(0) ;
    cvNamedWindow("Webcam",0);
    while(1)
    {
        frame = cvQueryFrame(capture);
        cvShowImage("Webcam",frame);
        if(cvWaitKey(10)>=0)
        {
              break;
        }
    }
    cvReleaseCapture(&capture);
    cvDestroyWindow("Webcam");
}
