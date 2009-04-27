#include "videowidget.h"

#include "ancaconf.h"

#include <qwidget.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qslider.h>
#include <qdrawutil.h>
#include <qapplication.h>
#include <qcursor.h>

#include <ptlib/vconvert.h>

#ifdef Q_WS_QWS
# define HAVE_PIXMAP_BITS
# define USE_DIRECT_PAINTER
# include <qdirectpainter_qws.h>
# include <qgfxraster_qws.h>
# include <qgfx_qws.h>
#endif

#define UPDATE_SIZE_TYPE 65431

/****************** VideoOutput ************************/

VideoOutput::VideoOutput( VideoWidget *parent ): QWidget(parent), parentWidget(parent) 
{
}

VideoOutput::~VideoOutput() {}

void VideoOutput::paintEvent( QPaintEvent * )
{	
	PTRACE(6, "VideoOutput::paintEvent");

	// Draw the current frame
	parentWidget->showFrame();
}

void VideoOutput::mouseReleaseEvent( QMouseEvent * )
{
//	parentWidget->setNextMode();
}

/****************** VideoWidget ************************/

VideoWidget::VideoWidget( QWidget *parent, const char *name )
: QHBox(parent,name), targetRect( 0, 0, 0, 0 ), screenMode( Normal ),
  videoDevice(0), videoOutput(this)
{
	// Create a QImage which is large enough to hold a video frame no matter what the
	// orientation of the screen or the movie is or regardless of any rotations in effect
	int size = QMAX( qApp->desktop()->width(), qApp->desktop()->height() );
	currentFrame = new QImage( size, size, 32 );
	rotatedFrame = new QImage( size, size, 32 );

	scaleContextDepth = -1;
	scaleContextInputWidth = -1;
	scaleContextInputHeight = -1;
	scaleContextPicture1Width = -1;
	scaleContextPicture2Width = -1;
	scaleContextOutputWidth = -1;
	scaleContextOutputHeight = -1;
	scaleContextLineStride = -1;
	clipContextPlaneSize = -1;

	ancaConf->installNotify( "scaled", this, SLOT(updateVideoOutput()), "videoout" );
	ancaConf->installNotify( "clipped", this, SLOT(updateVideoOutput()), "videoout" );
}

VideoWidget::~VideoWidget() 
{
	if ( currentFrame )
		delete currentFrame;
	delete rotatedFrame;

	if( clipContextPlaneSize != -1 ) {
		delete picture.data[0];
		delete picture.data[1];
		delete picture.data[2];
	}
}


bool VideoWidget::showFrame( const uchar *frame, int width, int height ) 
{
	PTRACE(8,"VideoWidget::showFrame: width " << width << ", height " << height);

	if( !isVisible() )
		return true;

	bool useDirectFrameBuffer = FALSE;
	bool rotateMovie90 = FALSE;
	bool rotateFullscreen90 = FALSE;

	int movieWidth = width;
	int movieHeight = height;

	int scaleWidth;
	int scaleHeight;

	int targetX;
	int targetY;
	int targetWidth;
	int targetHeight;

	int decodeX;
	int decodeY;
	int decodeWidth;
	int decodeHeight;

	QRect innerMovieArea = videoOutput.geometry();
#ifdef Q_WS_QWS
	bool screenRotated = (qt_screen->transformOrientation() & 1);
	int qt_screen_width = qt_screen->width();
	int qt_screen_height = qt_screen->height();
#else
	bool screenRotated = false;
#endif

	bool scaled = ancaConf->readBoolEntry("videoout", "scaled", true);

	bool needRotation = rotateMovie90 != rotateFullscreen90;
	bool needRotationWithScreen = screenRotated != needRotation;

	// Work out the target area
	if ( screenMode == Fullscreen ) {
		int fullscreenWidth = qApp->desktop()->width();
		int fullscreenHeight = qApp->desktop()->height();
		/*
		    If the device has a portrait screen, rotate in fullscreen mode
		    so that the maximum size of the movie can be displayed (which is
		    usually what fullscreen mode is for :)
		*/
		targetX = 0;
		targetY = 0;
		if ( fullscreenHeight > fullscreenWidth ) {
			rotateFullscreen90 = TRUE;
			targetWidth = fullscreenHeight;
			targetHeight = fullscreenWidth;
		} else {
			targetWidth = fullscreenWidth;
			targetHeight = fullscreenHeight;
		}
	} else {
		// Non-fullscreen case
		targetX = 0; //or innerMovieArea.left();
		targetY = 0; //or innerMovieArea.top();
		targetWidth = innerMovieArea.width();
		targetHeight = innerMovieArea.height();
	}

	// prevent divide by zero
	movieHeight = (movieHeight == 0) ? 1 : movieHeight;
	movieWidth = (movieWidth == 0) ? 1 : movieWidth;

	if ( movieHeight > movieWidth ) {
		/*
		    Movie has a portrait orientation, this is not normal for movie clips.
		    From experience downloading movie files from the internet designed
		    for PocketPC devices with such orientations are actually rotated and meant to
		    be rotated back by the software to a landscape orientation.
		    Examples encoded this way can be found at http://www.pocketmatrix.com/multimedia/
		    Presumably they are designed to be played with the 'Pocket DivX' movie player.
		*/
		rotateMovie90 = TRUE;
		decodeWidth = movieHeight;
		decodeHeight = movieWidth;
	} else {
		decodeWidth = movieWidth;
		decodeHeight = movieHeight;
	}

	if ( scaled ) {
		decodeX = 0;
		decodeY = 0;
		// maintain aspect ratio
		if ( targetHeight * decodeWidth > decodeHeight * targetWidth ) {
			scaleWidth = targetWidth;
			scaleHeight = decodeHeight * targetWidth / decodeWidth;
		} else {
			scaleWidth = decodeWidth * targetHeight / decodeHeight;
			scaleHeight = targetHeight;
		}
	} else {
		// Must ensure that the decoded pixels stays within the target frame
		movieWidth = decodeWidth;
		movieHeight = decodeHeight;
		decodeWidth  = QMIN( movieWidth, targetWidth );
		decodeHeight = QMIN( movieHeight, targetHeight );
		scaleWidth = decodeWidth;
		scaleHeight = decodeHeight;
		decodeX = (movieWidth - decodeWidth) / 2; 
		decodeY = (movieHeight - decodeHeight) / 2;
	}

	/*
	    At this stage we now have the following variables setup with the following meanings:
	
	        movieWidth		- The source width of the movie (after any portrait to landscape correction)
	        movieHeight		- The source height of the movie (after any portrait to landscape correction)
	        rotateMovie90		- Flag which sets if there needs to be portrait to landscape correction
	
	        scaleWidth		- This is the required target scaled size (with correct aspect ratio) to output
	        scaleHeight		- This is the required target scaled size (with correct aspect ratio) to output
	        rotateFullscreen90	- Flag which specifies if the movie fits best rotated in fullscreen mode
	
	        targetWidth		- The area to display in to
	        targetHeight		- The area to display in to
	*/

	int x1 = targetX + (targetWidth - scaleWidth) / 2;
	int y1 = targetY + (targetHeight - scaleHeight) / 2;

	// rotate device coordinates
	if ( rotateFullscreen90 ) {
		int t = x1;
		x1 = y1;
		y1 = t;
		t = scaleHeight;
		scaleHeight = scaleWidth;
		scaleWidth = t;
		t = targetHeight;
		targetHeight = targetWidth;
		targetWidth = t;
	}

	imageFb.create( currentFrame->bits(), currentFrame->bytesPerLine(), 0, 0, scaleWidth, scaleHeight );
	rotatedBuffer.create( rotatedFrame->bits(), rotatedFrame->bytesPerLine(), 0, 0, scaleHeight, scaleWidth );
	FrameBuffer *fb = &imageFb;

#ifdef USE_DIRECT_PAINTER
	QDirectPainter dp( &videoOutput );

	// Don't use direct painter if there are no regions returned
	if ( dp.numRects() != 0 ) {
	// area we will decode in to in global untransformed coordinates
	QRect drawRect = QRect( mapToGlobal( QPoint( x1, y1 ) ), QSize( scaleWidth, scaleHeight ) );
	// area transformed in to device coordinates
	QRect deviceRect = qt_screen->mapToDevice( drawRect, QSize( qt_screen_width, qt_screen_height ) );
	// The top corner point of this area
	QPoint offset = deviceRect.topLeft();

	//qDebug("rect = %i %i %i %i", drawRect.x(), drawRect.y(), drawRect.width(), drawRect.height() );
	//qDebug("x: %i  y: %i", offset.x(),  offset.y() );
	directFb.create( dp.frameBuffer(), dp.lineStep(), offset.x(), offset.y(), dp.width(), dp.height() );

	QSize deviceSize = qt_screen->mapToDevice( QSize( qt_screen_width, qt_screen_height ) );

	int dd = QPixmap::defaultDepth();
	if (( dd == 16 || dd == 32 )) {
		QRegion imageRegion;

		// global untransformed coordinates
		imageRegion = drawRect;

		for ( int i = 0; i < dp.numRects(); i++ ) {
			// map from device coordinates and subtract from our output region
			imageRegion -= qt_screen->mapFromDevice( dp.rect(i), deviceSize );
			//qDebug("dp.rect(%i) = %i %i %i %i", i, dp.rect(i).x(), dp.rect(i).y(), dp.rect(i).width(), dp.rect(i).height() );
		}

		// if we have completely subtracted everything from our output region,
		// there is no clipping needed against the draw region so we use direct painting
		useDirectFrameBuffer = imageRegion.isEmpty();
		//useDirectFrameBuffer = FALSE; // Uncomment to test the non useDirectFrameBuffer case
	}

	if ( useDirectFrameBuffer && !needRotationWithScreen )
		fb = &directFb;
	}
#endif

/*
#ifdef Q_WS_QWS
	qDebug("rotateMovie90: %s  rotateFullscreen90: %s  needRotation: %s  needRotationWithScreen: %s  useDirectFrameBuffer: %s  transformOrientation: %i",
#else
	qDebug("rotateMovie90: %s  rotateFullscreen90: %s  needRotation: %s  needRotationWithScreen: %s  useDirectFrameBuffer: %s",
#endif
		rotateMovie90 ? "true" : "false",
		rotateFullscreen90 ? "true" : "false",
		needRotation ? "true" : "false",
		needRotationWithScreen ? "true" : "false",
#ifdef Q_WS_QWS
		useDirectFrameBuffer ? "true" : "false",
		qt_screen->transformOrientation() );
#else
		useDirectFrameBuffer ? "true" : "false" );
#endif

	qDebug("useDirectFrameBuffer: %s", useDirectFrameBuffer ? "true" : "false" );
*/

	// rotate decode coordinates
	if ( rotateMovie90 ) {
		// Swap arguments
		int t = decodeHeight;
		decodeHeight = decodeWidth;
		decodeWidth = t;
		t = decodeX;
		decodeX = decodeY;
		decodeY = t;
	}
	
	// rotate output coordinates
	if ( needRotation ) {
		int t = scaleHeight;
		scaleHeight = scaleWidth;
		scaleWidth = t;
	}

#ifdef HAVE_PIXMAP_BITS
	QPixmap *pix = 0;
	FrameBuffer pixFb;
	if ( !useDirectFrameBuffer ) {
		if ( needRotation ) {
			if ( !needRotationWithScreen ) {
				pix = new QPixmap( qt_screen->mapToDevice( QSize( scaleWidth, scaleHeight ) ) );
				pixFb.create( pix->scanLine(0), pix->bytesPerLine(), 0, 0, scaleWidth, scaleHeight );
				fb = &pixFb;
			}
		}
	}
#endif

/*
	qDebug( "frameWidth %d, frameHeight %d, decodeX %d, decodeY %d, decodeWidth %d, decodeHeight %d, scaleWidth %d, scaleHeight %d\n",
			width,
			height,
			decodeX,
			decodeY,
			decodeWidth,
			decodeHeight,
			scaleWidth,
			scaleHeight );
*/

	qApp->lock();
	QPainter p( &videoOutput );

	QRect targetArea = QRect( x1, y1, scaleWidth, scaleHeight );
	// Image changed size, therefore need to blank the target region first
	if ( targetRect != targetArea ) {
		p.setBrush( palette().active().background() );
		p.setPen( palette().active().background() );

		p.drawRect( QRect( targetX, targetY, targetWidth, targetHeight ) );

		targetRect = targetArea;
	}

	scaleFrame( frame, width, height, fb->jumpTable(), decodeX, decodeY, decodeWidth, decodeHeight, scaleWidth, scaleHeight);

//	qApp->lock();

	// ### I think I found a case which is broken, screenRotated=FALSE and rotateMovie90=TRUE and rotateFullscreen90=FALSE and useDirectFrameBuffer=FALSE, image is rotated 180
	// and when screenRotated=TRUE for this case, it can crash

	if ( !useDirectFrameBuffer ) {
		if ( needRotation ) {
			//qDebug("case 1");
			// eg PDA 240x320, you really want to view it rotated long-wise
#ifdef HAVE_PIXMAP_BITS
			if ( !needRotationWithScreen ) {
				p.drawPixmap( x1, y1, *pix );
			} else
#endif
			{
				fb->rotateToBuffer( &rotatedBuffer, 0, 0, 0, 0, scaleWidth, scaleHeight, !rotateMovie90 );
				p.drawImage( x1, y1, *rotatedFrame, 0, 0, scaleHeight, scaleWidth );
			}
		} else {
			//qDebug("case 2");
			// eg 640x480 device, it is best not to rotate image around
			p.drawImage( x1, y1, *currentFrame, 0, 0, scaleWidth, scaleHeight );
		}
	} else {
		if ( needRotationWithScreen ) {
			//qDebug("case 3");
			// Work out if will need to rotate clockwise or anticlockwise by 90 degrees
#ifdef Q_WS_QWS				
			bool screenRotatedMoreThan180 = (qt_screen->transformOrientation() & 2);
#else
			bool screenRotatedMoreThan180 = false;
#endif
			fb->rotateToBuffer( &directFb, 0, 0, 0, 0, scaleWidth, scaleHeight, !screenRotatedMoreThan180 );
		} else {
			//qDebug("case 4");
		}
	}

#ifdef HAVE_PIXMAP_BITS
	delete pix;
#endif

	p.flush();
	qApp->unlock();

	return true;
}

void VideoWidget::showFrame() 
{
	// I suppose that this is called only from the main GUI thread

	if( videoDevice ) {
		videoDevice->EndFrame();
	}
	else {
		QPainter p(&videoOutput);

		QPixmap logo = QPixmap::fromMimeSource("anca.png");
		if( !logo.isNull() ) {
			int width = logo.width();
			int height = logo.height();

			p.drawPixmap( (videoOutput.width() - width) / 2,
				    (videoOutput.height() - height) / 2,
				    logo );
		}
		else {
			//draw this string
			QString noSignal = "No Signal"; //TODO configure
	
			p.setFont( QFont( "Arial", 12, QFont::Bold ) ); //TODO configure
			int width = p.fontMetrics().width( noSignal );
			int height = p.fontMetrics().height();
	
			p.drawText( (videoOutput.width() - width) / 2,
			            (videoOutput.height() - height) / 2,
				    noSignal );
		}
	}
}

void VideoWidget::updateVideoOutput() 
{
	PTRACE( 6, "VideoWidget::updateVideoOutput" );

	bool scaled = ancaConf->readBoolEntry("videoout", "scaled", true);
	bool clipped = ancaConf->readBoolEntry("videoout", "clipped", true);

	if( !scaled && !clipped && videoDevice ) {
		videoOutput.setMinimumWidth( QMIN((int)videoDevice->GetFrameWidth(), qApp->desktop()->width()) );
		videoOutput.setMinimumHeight( QMIN((int)videoDevice->GetFrameHeight(), qApp->desktop()->height()) );
	}
	else {
		videoOutput.setMinimumWidth( 1 );
		videoOutput.setMinimumHeight( 1 );
	}

	videoOutput.updateGeometry();
}

void VideoWidget::scaleFrame( const uchar *frame, int frame_w, int frame_h, uchar **output_rows, int clip_x, int clip_y, int clip_w, int clip_h, int scale_w, int scale_h ) 
{
	PTRACE(8,"VideoWidget::scaleFrame" );

	int colorMode = MODE_32_RGB;
	
	if ( colorMode != scaleContextDepth ) {
	    scaleContextDepth = colorMode;
	    videoScaleContext = yuv2rgb_factory_init( colorMode, 0, 0 );
	}
	
	int lineStride = (uchar*)output_rows[1] - (uchar*)output_rows[0];
	
	clipFrame(frame, frame_w, frame_h, &picture, clip_x, clip_y, clip_w, clip_h);
	// Check if any colour space conversion variables have changed
	// since the last decoded frame which will require
	// re-initialising the colour space tables 
	if ( scaleContextInputWidth != clip_w ||
	     scaleContextInputHeight != clip_h ||
	     scaleContextPicture1Width != picture.linesize[0] ||
	     scaleContextPicture2Width != picture.linesize[1] ||
	     scaleContextOutputWidth != scale_w ||
	     scaleContextOutputHeight != scale_h ||
	     scaleContextLineStride != lineStride ) {
		
		scaleContextInputWidth = clip_w;
		scaleContextInputHeight = clip_h;
		scaleContextPicture1Width = picture.linesize[0];
		scaleContextPicture2Width = picture.linesize[1];
		scaleContextOutputWidth = scale_w;
		scaleContextOutputHeight = scale_h;
		scaleContextLineStride = lineStride;
		
		int format = FORMAT_YUV420;
		
//		qDebug("reconfiguring scale context");
		videoScaleContext->converter->configure( videoScaleContext->converter,
		clip_w, clip_h, picture.linesize[0], picture.linesize[1], scale_w, scale_h, lineStride, format );
//		qDebug("configured yuv convert context with - input: %i x %i  pic lines: %i %i, output: %i x %i, linestride: %i", clip_w, clip_h, picture.linesize[0], picture.linesize[1], scale_w, scale_h, lineStride );
	}
    
	videoScaleContext->converter->yuv2rgb_fun( videoScaleContext->converter, (uint8_t*)output_rows[0], picture.data[0], picture.data[1], picture.data[2] );
}

void VideoWidget::clipFrame( const uchar *frame, int frame_w, int frame_h, AVPicture *picture, int clip_x, int clip_y, int clip_w, int clip_h) 
{
	PTRACE(8,"VideoWidget::clipFrame" );

	int planeSize = frame_w * frame_h;
	int planeSize2 = planeSize >> 2;
	const uchar *y_plane = frame;
	const uchar *u_plane = y_plane + planeSize;
	const uchar *v_plane = u_plane + planeSize2;

	// do not clip, just prepare 'picture'
	if( clip_x == 0 && clip_y == 0 ) {
		if( clipContextPlaneSize != -1 ) {
			delete picture->data[0];
			delete picture->data[1];
			delete picture->data[2];
		}
		clipContextPlaneSize = -1;
		picture->data[0] = (uchar*)y_plane;
		picture->data[1] = (uchar*)u_plane;
		picture->data[2] = (uchar*)v_plane;
		picture->linesize[0] = frame_w;
		picture->linesize[1] = frame_w >> 1;
		picture->linesize[2] = frame_w >> 1;
	}
	// clip the frame. The output of the algorithm is the clipped frame in the middle
	else {
		int cPlaneSize = clip_w * clip_h; //clipped plane size
		int cPlaneSize2 = cPlaneSize >> 2;

		if( clipContextPlaneSize != -1 && clipContextPlaneSize != cPlaneSize ) {
			delete picture->data[0];
			delete picture->data[1];
			delete picture->data[2];
		}
		if( clipContextPlaneSize != cPlaneSize ) {
			picture->data[0] = new uchar[cPlaneSize];
			picture->data[1] = new uchar[cPlaneSize2];
			picture->data[2] = new uchar[cPlaneSize2];
			clipContextPlaneSize = cPlaneSize;
		}

		int linestep_out    = clip_w;
		int linestep_in     = frame_w;
		int startline_in    = clip_y;
		int startcolumn_in  = clip_x;
		int lines           = clip_h;

		// clip y plane
		const uchar *in = y_plane + startline_in*linestep_in;
		uchar *out = picture->data[0];
		for( int i = 0; i < lines; i++ ) {
			memcpy( out, in + startcolumn_in, linestep_out );
			out += linestep_out;
			in  += linestep_in;
		}
		picture->linesize[0] = linestep_out;

		linestep_out    = clip_w >> 1;
		linestep_in     = frame_w >> 1;
		startline_in    = clip_y >> 1;
		startcolumn_in  = clip_x >> 1;
		lines           = clip_h >> 1;

		// clip u plane
		in = u_plane + startline_in*linestep_in;
		out = picture->data[1];
		for( int i = 0; i < lines; i++ ) {
			memcpy( out, in + startcolumn_in, linestep_out );
			out += linestep_out;
			in  += linestep_in;
		}
		picture->linesize[1] = linestep_out;

		// clip v plane
		in = v_plane + startline_in*linestep_in;
		out = picture->data[2];
		for( int i = 0; i < lines; i++ ) {
			memcpy( out, in + startcolumn_in, linestep_out );
			out += linestep_out;
			in  += linestep_in;
		}
		picture->linesize[2] = linestep_out;
	}
}

void VideoWidget::setNextMode()
{
	if ( screenMode == Fullscreen ) 
		setMode( Normal );
	else
		setMode( Fullscreen );
}

void VideoWidget::makeVisible()
{
	// this must be called only from the main thread!

	videoOutput.hide();

	if ( screenMode == Fullscreen ) {
		videoOutput.reparent( 0, QPoint( 0, 0 ) );
		videoOutput.resize( qApp->desktop()->size() );
		videoOutput.setCursor( QCursor( blankCursor ) );
		videoOutput.showFullScreen();
	} else {
		videoOutput.reparent( this, QPoint(0,0) );
		videoOutput.unsetCursor();
		videoOutput.show();
	}
}

void VideoWidget::setMode( int mode )
{
	if ( screenMode == (Mode)mode )
		return;

	screenMode = (Mode)mode;

	// toggle from/to Fullscreen from/to Normal
	makeVisible();
}

PVideoOutputDevice *VideoWidget::getVideoDevice()
{
	return videoDevice;
}

void VideoWidget::customEvent( QCustomEvent *e )
{
	if( e->type() == UPDATE_SIZE_TYPE )
		updateVideoOutput();
}

void VideoWidget::setVideoDevice( VideoOutputDevice *device )
{
	videoDevice = device;
	qApp->postEvent(this, new QCustomEvent(UPDATE_SIZE_TYPE));
}


/***************** VideoOutputDevice ****************/

VideoOutputDevice::VideoOutputDevice( VideoWidget* video): videoWidget(video) 
{
	int planeSize = frameWidth * frameHeight;
	frameStore.SetSize( planeSize + 2*(planeSize>>2));
}

VideoOutputDevice::~VideoOutputDevice()
{
	if( isOpened ) Close();
}

BOOL VideoOutputDevice::Open( const PString &, BOOL ) {
	PTRACE(6,"VideoOutputDevice::Open" );

	isOpened = true;
	videoWidget->setVideoDevice(this);

	return true;
}

BOOL VideoOutputDevice::IsOpen() {
	return isOpened;
}

BOOL VideoOutputDevice::Close() {
	PTRACE(6,"VideoOutputDevice::Close" );

	isOpened = false;
	videoWidget->setVideoDevice(0);

	return true;
}

PStringList VideoOutputDevice::GetDeviceNames() const {
	PStringList  devlist;
	devlist.AppendString(GetDeviceName());
	
	return devlist;
}

BOOL VideoOutputDevice::CanCaptureVideo() const {
	return false;
}

BOOL VideoOutputDevice::EndFrame() {
	return videoWidget->showFrame( (const uchar*)frameStore, frameWidth, frameHeight );
}

BOOL VideoOutputDevice::SetColourFormat ( const PString & colour_format ) {
	if (colour_format == "YUV420P")
		return PVideoOutputDevice::SetColourFormat (colour_format);

	return FALSE;  
}

BOOL VideoOutputDevice::SetFrameData(unsigned, unsigned, unsigned width, unsigned height, const BYTE * data, BOOL endFrame) {
	PTRACE(8,"SetFrameData: " << PString(width) << " x " << PString(height));

	if (width * height > frameWidth * frameHeight)
		return FALSE;

	int planeSize = frameWidth * frameHeight;
	qApp->lock();
	memcpy(frameStore.GetPointer(), data, planeSize + 2*(planeSize>>2));
	qApp->unlock();

	if( endFrame )
		return EndFrame();

	return true;
}

BOOL VideoOutputDevice::SetFrameSize(unsigned width, unsigned height) {
	PTRACE(6,"VideoOutputDevice::SetFrameSize" );

	if (!PVideoOutputDevice::SetFrameSize(width, height))
		return FALSE;
	
	// frame is in YUV420, that means
	// Y: 1B per pixel
	// U: 0.5B per 2 pixels
	// V: 0.5B per 2 pixels
	int planeSize = frameWidth * frameHeight;
	return frameStore.SetSize(planeSize + 2*(planeSize>>2));
}

PINDEX VideoOutputDevice::GetMaxFrameBytes()
{
	return frameStore.GetSize();
}

