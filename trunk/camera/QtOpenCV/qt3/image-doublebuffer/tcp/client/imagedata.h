
#ifndef IMAGEDATA_H
#define IMAGEDATA_H

#define BUFFERSIZE 1920
struct imagedata
{
	unsigned char r_pixel[480][640];
	unsigned char g_pixel[480][640];
	unsigned char b_pixel[480][640];
};

#endif
