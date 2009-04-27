/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <ptlib.h>

#include <qwidget.h>
#include <qslider.h>
#include <qlineedit.h>
#include <qrect.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qimage.h>
#include <qhbox.h>
#include <qwaitcondition.h>

#include "framebuffer.h"

#include "ancaconf.h"

#include "yuv2rgb.h"

class VideoWidget;

class VideoOutput: public QWidget
{
	Q_OBJECT

public:
	VideoOutput( VideoWidget *parent );
	~VideoOutput();

protected:
	void paintEvent( QPaintEvent *event );
	void mouseReleaseEvent( QMouseEvent *me );

private:
	VideoWidget *parentWidget;
};

class VideoOutputDevice : public PVideoOutputDevice
{
	PCLASSINFO( VideoOutputDevice, PVideoOutputDevice );

public:
	VideoOutputDevice( VideoWidget* videoWidget );
	~VideoOutputDevice();

	/**
	 * Open the device given the device name
	 * @see PVideoDevice
	 */
	virtual BOOL Open( const PString & deviceName, BOOL startImmediate = TRUE );

	/**
	 * Determine if the device is currently open
	 * @see PVideoDevice
	 */
	virtual BOOL IsOpen();
	
	/**
	 * Close the device
	 */
	virtual BOOL Close();

	/**
	 * Get a list of all of the drivers available.
	 * @see PVideoDevice
	 */
	virtual PStringList GetDeviceNames() const;

	/**
	 * Is the device a camera, and obtain video
	 * @see PVideoOutputDevice
	 */
	virtual BOOL CanCaptureVideo() const;

	/**
	 * Indicate frame may be displayed
	 * @see PVideoOutputDevice
	 */
	virtual BOOL EndFrame();

	/**
	 * Set the colour format to be used.
	 * @see PVideoOutputDeviceRGB
	 */
	virtual BOOL SetColourFormat ( const PString & colourFormat );

	/**
	 * Set a section of the output frame buffer.
	 * @see PVideoOutputDeviceRGB
	 */
	virtual BOOL SetFrameData( unsigned x, unsigned y, unsigned width, unsigned height, const BYTE * data, BOOL endFrame );

	/**
	 * Set the frame size to be used.
	 */
	virtual BOOL SetFrameSize( unsigned width, unsigned height );
	
	/**
	 * Get the maximum frame size in bytes.       
	 */
	virtual PINDEX GetMaxFrameBytes();
	
protected:
	PBYTEArray frameStore;

	QWaitCondition frameShown;

private:
	VideoWidget *videoWidget;
	bool isOpened;
};

class VideoWidget: public QHBox
{
	Q_OBJECT
	friend class VideoOutputDevice;

public:
	VideoWidget( QWidget *parent = 0, const char *name = 0);
	~VideoWidget();

	bool showFrame( const uchar *frame, int width, int height );
	void showFrame();

	PVideoOutputDevice *getVideoDevice();

	enum Mode {
	    InvalidMode,
	    Fullscreen,
	    Normal
	};

	Mode mode() { return screenMode; }

	void setPaletteColors( const QPalette& pal );

public slots:
	void makeVisible();
	void setMode( int );
	void setNextMode();

	void updateVideoOutput();

protected:
	void customEvent( QCustomEvent * );
	void setVideoDevice( VideoOutputDevice * );
	
private:
	void scaleFrame( const uchar *frame, int frame_w, int frame_h, uchar **dest, int clip_x, int clip_y, int clip_w, int clip_h, int scale_w, int scale_h );

	QRect targetRect;
	QImage *currentFrame;
	QImage *rotatedFrame;
	FrameBuffer imageFb;
	FrameBuffer rotatedBuffer;
	FrameBuffer directFb;

	Mode screenMode;

	VideoOutputDevice *videoDevice;
	
	VideoOutput videoOutput;

	/**
	 * four components are given, that's all.
	 * the last component is alpha
	 */
	typedef struct AVPicture
	{
		UINT8 *data[ 4 ];
		int linesize[ 4 ];
	}
	AVPicture;

	void clipFrame( const uchar *frame, int frame_w, int frame_h, AVPicture *picture, int clip_x, int clip_y, int clip_w, int clip_h );

	int scaleContextDepth;
	int scaleContextInputWidth;
	int scaleContextInputHeight;
	int scaleContextPicture1Width;
	int scaleContextPicture2Width;
	int scaleContextOutputWidth;
	int scaleContextOutputHeight;
	int scaleContextLineStride;
	int scaleContextFormat;
	int clipContextPlaneSize;
	AVPicture picture;
	yuv2rgb_factory_t	*videoScaleContext;
};

#endif
