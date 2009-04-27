#include "fakedevice.h"

/**
 * RGBtoYUV420PSameSize function is from Gnomemeeting
 */
#define rgbtoyuv(r, g, b, y, u, v) \
  y=(BYTE)(((int)30*r  +(int)59*g +(int)11*b)/100); \
  u=(BYTE)(((int)-17*r  -(int)33*g +(int)50*b+12800)/100); \
  v=(BYTE)(((int)50*r  -(int)42*g -(int)8*b+12800)/100); \

static void RGBtoYUV420PSameSize (const BYTE * rgb,
				  BYTE * yuv,
				  unsigned rgbIncrement,
				  BOOL flip, 
				  int srcFrameWidth, int srcFrameHeight) 
{
  const unsigned planeSize = srcFrameWidth*srcFrameHeight;
  const unsigned halfWidth = srcFrameWidth >> 1;
  
  // get pointers to the data
  BYTE * yplane  = yuv;
  BYTE * uplane  = yuv + planeSize;
  BYTE * vplane  = yuv + planeSize + (planeSize >> 2);
  const BYTE * rgbIndex = rgb;

  for (int y = 0; y < (int) srcFrameHeight; y++) {
    BYTE * yline  = yplane + (y * srcFrameWidth);
    BYTE * uline  = uplane + ((y >> 1) * halfWidth);
    BYTE * vline  = vplane + ((y >> 1) * halfWidth);

    if (flip)
      rgbIndex = rgb + (srcFrameWidth*(srcFrameHeight-1-y)*rgbIncrement);

    for (int x = 0; x < (int) srcFrameWidth; x+=2) {
      rgbtoyuv(rgbIndex[0], rgbIndex[1], rgbIndex[2],*yline, *uline, *vline);
      rgbIndex += rgbIncrement;
      yline++;
      rgbtoyuv(rgbIndex[0], rgbIndex[1], rgbIndex[2],*yline, *uline, *vline);
      rgbIndex += rgbIncrement;
      yline++;
      uline++;
      vline++;
    }
  }
}



FakeVideoInputDevice::FakeVideoInputDevice( const QImage& image ): image(image)
{
	if( this->image.isNull() ) {
		this->image = QImage::fromMimeSource("anca.png");
	}
}


FakeVideoInputDevice::~FakeVideoInputDevice ()
{
}


BOOL FakeVideoInputDevice::GetFrameDataNoDelay (BYTE *frame, PINDEX *)
{
	QImage tmpImage;

	unsigned width = 0;
	unsigned height = 0;
	
	GetFrameSize (width, height);
	
	grabCount++;
	
	tmpImage = image.scale(width, height);
	
	RGBtoYUV420PSameSize( tmpImage.bits(), frame, tmpImage.depth() / 8, FALSE, width, height);
	
	return TRUE;
}
