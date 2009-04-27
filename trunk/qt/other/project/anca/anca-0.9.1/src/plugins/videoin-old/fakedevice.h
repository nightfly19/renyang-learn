#ifndef FAKEDEVICE_H
#define FAKEDEVICE_H

#include <ptlib.h>
#include <qimage.h>

class FakeVideoInputDevice : public PFakeVideoInputDevice 
{
	PCLASSINFO(FakeVideoInputDevice, PFakeVideoInputDevice);

public:
	FakeVideoInputDevice( const QImage& image );

	~FakeVideoInputDevice ();

	virtual BOOL GetFrameDataNoDelay (BYTE *, PINDEX *);

protected:
	QImage image;
};

#endif
