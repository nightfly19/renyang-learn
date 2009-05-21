/* 
 * fdiff.c
 * frame subtraction difference
 *
 * 2006-09-28 tmori 
 * 2006-09-24 tmori 
 * 2006-09-18 tmori 
 * 
 */
#include <stdio.h>
#include <ctype.h> /* for isdigit */
#include "cv.h"
#include "highgui.h"


#define DIFFERENCE_THRESHOLD 5


int bytes_per_pixel(const IplImage* image);
void process(IplImage *source, IplImage *destination);


IplImage* previous0 = 0;
IplImage* previous1 = 0;


/* main */
int main( int argc, char** argv ) {
  CvCapture* capture = 0;
  IplImage* image = 0;
  IplImage* result = 0;
  int tick=0, prev_tick=0;
  double now=0.0;
  CvFont font;
  char buffer[256];

  
  if( argc == 1 
      || (argc == 2 && strlen(argv[1]) == 1 && isdigit(argv[1][0]))) {
    capture = cvCreateCameraCapture( argc == 2 ? argv[1][0] - '0' : 1 );
  }
  else if( argc == 2 ) {
    capture = cvCreateFileCapture( argv[1] ); 
  }
  if( !capture ) {
    fprintf( stderr, "ERROR: capture is NULL \n" );
    return(-1);
  }
  
  cvNamedWindow( "Operation", CV_WINDOW_AUTOSIZE );
  cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1.0, 1.0, 0.0, 1, 0);
  image = cvQueryFrame( capture );
  if( !image ) {
    fprintf(stderr,"Could not query frame...\n");
    return(-1);
  }
  result = cvCreateImage( cvSize(image->width, image->height),
			  IPL_DEPTH_8U, 3);
  previous0 = cvCreateImage( cvSize(image->width, image->height),
			  IPL_DEPTH_8U, 3);
  previous1 = cvCreateImage( cvSize(image->width, image->height),
			  IPL_DEPTH_8U, 3);
  
  for(;;) {
    image = cvQueryFrame( capture );
    if( !image ) {
      fprintf(stderr, "Could not query frame...\n");
      break;
    }
    
    process(image, result);
    
    sprintf(buffer, "%3.1lfms", now/1000);
    cvPutText(result, buffer, cvPoint(50, 150), &font, CV_RGB(255, 0, 0));

    cvShowImage("Operation", result );
    
    //If a certain key pressed
    if( cvWaitKey(10) >= 0 ) {
      break;
    }

    tick = cvGetTickCount();
    now = (tick - prev_tick) / cvGetTickFrequency();
    prev_tick = tick;
  }
  
  cvReleaseImage( &previous1 );
  cvReleaseImage( &previous0 );
  cvReleaseImage( &result );

  cvReleaseCapture( &capture );
  cvDestroyWindow("Operation");
  
  return( 0 );
}


/* bytes_per_pixel */
int bytes_per_pixel(const IplImage* image) {
  return((((image)->depth & 255) / 8 ) *(image)->nChannels);
}


/* process */
void process(IplImage *source, IplImage *destination) {
  uchar *pS, *pD, *pP0, *pP1;
  uchar *dataS, *dataD, *dataP0, *dataP1;
  int bpp;
  int step;
  CvSize size;
  int x, y;
  double intensityS;
  int intensityS_d;
  double intensityP0;
  int intensityP0_d;
  double intensityP1;
  int intensityP1_d;
  int intensity_diff0;
  int intensity_diff1;

  /* for outer region */
  cvCopy( source, destination, NULL );

  /* bpp 3 */
  bpp = bytes_per_pixel(source);

  /* step 1920, size 640,480 */
  cvGetRawData(source, &dataS, &step, &size);    
  cvGetRawData(destination, &dataD, NULL, NULL);
  cvGetRawData(previous0, &dataP0, NULL, NULL);
  cvGetRawData(previous1, &dataP1, NULL, NULL);

  /* inner region */
  for(y=size.height/8; y<(size.height*7/8); y++) {
    pS = dataS+step*size.height/8+bpp*size.width/8;
    pD = dataD+step*size.height/8+bpp*size.width/8;
    pP0 = dataP0+step*size.height/8+bpp*size.width/8;
    pP1 = dataP1+step*size.height/8+bpp*size.width/8;
    
    for(x=size.width/8; x<(size.width*7/8); x++) {
      intensityS = (0.114 * pS[0] + 0.587 * pS[1] + 0.299 * pS[2]) / 255.0;
      intensityS_d = (int)(219.0 * intensityS) + 16;
      intensityP0 = (0.114 * pP0[0] + 0.587 * pP0[1] + 0.299 * pP0[2]) / 255.0;
      intensityP0_d = (int)(219.0 * intensityP0) + 16;
      intensityP1 = (0.114 * pP1[0] + 0.587 * pP1[1] + 0.299 * pP1[2]) / 255.0;
      intensityP1_d = (int)(219.0 * intensityP1) + 16;

      intensity_diff0 = intensityS_d - intensityP0_d;
      intensity_diff0 = ((intensity_diff0<0) ? -intensity_diff0 : intensity_diff0);
      intensity_diff1 = intensityP0_d - intensityP1_d;
      intensity_diff1 = ((intensity_diff1<0) ? -intensity_diff1 : intensity_diff1);

      if( (intensity_diff0 > DIFFERENCE_THRESHOLD) 
	  && (intensity_diff1 > DIFFERENCE_THRESHOLD) ) {
	*pD = pP0[0];
	*(pD+1) = pP0[1];
	*(pD+2) = pP0[2];
      }
      else {
	*pD = 0;
	*(pD+1) = 0;
	*(pD+2) = 0;
      }
	
      pS += bpp; // next pixel of the source
      pD += bpp; // next pixel of the destination
      pP0 += bpp; // next pixel of the source
      pP1 += bpp; // next pixel of the source
    }
    dataS += step; // next line of the source
    dataD += step; // next line of the destination
    dataP0 += step; // next line of the source
    dataP1 += step; // next line of the source
  }    

  /* for next */
  cvCopy( previous0, previous1, NULL );
  cvCopy( source, previous0, NULL );
}



