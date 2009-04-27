#include "videograbber.h"
#include "ancaconf.h"
#include "ancainfo.h"
#include "stdinfo.h"
//#include "fakedevice.h"

#include <qapplication.h>
#include <qimage.h>

VideoGrabber *videoGrabber = 0;

VideoGrabber::VideoGrabber( PConfigArgs *args ) 
: PThread( 1000, PThread::NoAutoDeleteThread ), videoOutputDeviceCreate( 0 ),
  videoReader(0), videoPlayer(0), args(args), bufferSize(0), buffer(0)
{
	videoGrabber = this;

	channel = new PVideoChannel();

	run = true;
	reconfigure = false;
	preparing = false;
	state = SSleep;

	Resume();
}

VideoGrabber::~VideoGrabber()
{
	closeDevices();
	delete channel;
	delete buffer;
	videoGrabber = 0;
}

void VideoGrabber::configurationChanged()
{
	PTRACE( 6, "VideoGrabber::configurationChanged" );

	PWaitAndSignal locker( mutex );
	reconfigure = true;
}

void VideoGrabber::Main()
{
	if ( !channel ) return ;

	PTRACE( 1, "Going to grab from video device" );
	while ( run ) {
		mutex.Wait();
		State s = state;
		mutex.Signal();

		// prepare for the state...
		closeDevices();
		switch( s ) {
			case SPreview:
				if( !openDevices() ) {
					stopPreview();
					ancaInfo->setInt( VG_STATE, VG_IDLE );
				}
				break;
			case SCall:
				openReader();
				break;
			case SSleep:
				break;
		}
		preparing = false;

		// do
		switch( s ) {
			case SSleep:
			case SCall:
				goSleep();
				break;
			case SPreview:
				while ( state == SPreview && run && !reconfigure ) {
					channel->Read( buffer, bufferSize );
					channel->Write( buffer, bufferSize );

					Sleep( 50 );
				}
		}
	}
}

bool VideoGrabber::openDevices()
{
	PTRACE( 6, "VideoGrabber::openDevices" );

	if( !openReader() || !openPlayer() )
		return false;

	unsigned int w, h;
	videoReader->GetFrameSize( w, h );
	videoPlayer->SetFrameSize( w, h );

	if ( bufferSize && videoReader->GetMaxFrameBytes() > bufferSize )
		delete buffer;
	if ( !bufferSize || videoReader->GetMaxFrameBytes() > bufferSize ) {
		buffer = new char[ videoReader->GetMaxFrameBytes() ];
		bufferSize = videoReader->GetMaxFrameBytes();
		PTRACE( 6, "Grabber buffer set to size " << bufferSize );
	}

	return true;
}

bool VideoGrabber::openReader() {
	PAssert( !videoReader, "Opening video reader that already exists." );

	unsigned minw, minh, maxw, maxh, width, height;
	QImage image;

	ancaInfo->setInt( VG_STATE, VG_OPENING );

	QString videoDriver = ancaConf->readEntry( "videoin", "videoDriver", "V4L" );
	QString videoDevice = ancaConf->readEntry( "videoin", "videoDevice", (const char *)PVideoChannel::GetDefaultDevice(PVideoChannel::Recorder) );
	int videoChannel = ancaConf->readIntEntry( "videoin", "videoChannel", -1 );
	int videoFormat = ancaConf->readIntEntry( "videoin", "videoFormat", PVideoDevice::Auto );
	int videoFrameRate = ancaConf->readIntEntry( "videoin", "videoFrameRate", 0 );
	QString videoImage = ancaConf->readEntry( "videoin", "videoImage", QString::null );

	if( args->HasOption("videoformat") ) {
		PString tmp = args->GetOptionString("videoformat");
		if( tmp *= "auto" ) videoFormat = PVideoDevice::Auto;
		else if( tmp *= "pal" ) videoFormat = PVideoDevice::PAL;
		else if( tmp *= "ntsc" ) videoFormat = PVideoDevice::NTSC;
	}
	if( args->HasOption("videodevice") )
		videoDevice = (const char *)args->GetOptionString("videodevice");
	
	videoReader = PVideoInputDevice::CreateDevice( videoDriver.latin1() );
	if ( !videoReader ) {
		PTRACE( 1, "Allocation of videoReader failed!" );
		return false;
	} else PTRACE( 6, "VideoReader allocated" );
	
	if ( !videoReader->Open( videoDevice.latin1(), false ) ) {
		PTRACE( 1, "Open failed" );
		goto video_error;
	} else PTRACE( 6, "VideoReader opened" );
	
	if ( !videoReader->SetColourFormatConverter( "YUV420P" ) ) {
		PTRACE( 1, "SetColourFormatConverter failed" );
		goto video_error;
	} else PTRACE( 6, "SetColourFormatConverter succeeded" );
	
	if ( !videoReader->SetChannel( videoChannel ) ) {
		PTRACE( 1, "SetChannel failed" );
		goto video_error;
	} else PTRACE( 6, "SetChannel succeeded" );
	
	if ( !videoReader->SetVideoFormat( ( PVideoDevice::VideoFormat ) videoFormat ) ) {
		PTRACE( 1, "SetVideoFormat failed" );
		goto video_error;
	} else PTRACE( 6, "SetVideoFormat succeeded" );
	
	if ( !videoReader->SetFrameRate( videoFrameRate ) ) {
		PTRACE( 1, "SetFrameRate failed" );
		goto video_error;
	} else PTRACE( 6, "SetFrameRate succeeded" );
	
	videoReader->GetFrameSizeLimits( minw, minh, maxw, maxh );
	configureVideoFrameSize( maxw, maxh, width, height );
	if  (!videoReader->SetFrameSizeConverter(width, height, FALSE)) {
		PTRACE(1, "Failed to set framesize to " << width<< "x" << height);
		goto video_error;
	} else PTRACE( 6, "SetFrameSizeConverter succeeded to set frame to " << width<< "x" << height);
	
	goto start_video_reader;

video_error:
//	PTRACE( 1, "Error while opening video device. The chosen video image will be transmitted instead." );
	PTRACE( 1, "Error while opening video device." );
	delete videoReader;

//	if( args->HasOption("videoimage") )
//		videoImage = (const char *)args->GetOptionString("videoimage");
//	image.load(videoImage);
//	videoReader = new FakeVideoInputDevice( image );
//	videoReader->SetColourFormatConverter ("YUV420P");
//	videoReader->SetVideoFormat (PVideoDevice::PAL);
//	videoReader->SetChannel (1);    
//	videoReader->SetFrameRate (6);
//	videoReader->GetFrameSizeLimits( minw, minh, maxw, maxh );
//	configureVideoFrameSize( maxw, maxh, width, height );
//	videoReader->SetFrameSizeConverter (width, height, FALSE);

	return false;

start_video_reader:
	videoReader->Start();

	channel->AttachVideoReader( videoReader );
	
	ancaInfo->setInt( VG_STATE, VG_OPENED );
	
	return true;
}

bool VideoGrabber::openPlayer()
{
	PAssert( !videoPlayer, "Opening video player that already exists." );

	if( videoOutputDeviceCreate )
		videoPlayer = videoOutputDeviceCreate();

	if ( !videoPlayer ) {
		PTRACE( 1, "no video player set" );
		return false;
	}
	
	channel->AttachVideoPlayer( videoPlayer );
	
	return videoPlayer->Open( "", 0 );
}

void VideoGrabber::closeDevices()
{
	closePlayer();
	closeReader();
}

void VideoGrabber::closePlayer()
{
	PTRACE( 6, "VideoGrabber::closePlayer" );

	if( videoPlayer ) {
		channel->CloseVideoPlayer();
		videoPlayer = 0;
	}
}

void VideoGrabber::closeReader()
{
	PTRACE( 6, "VideoGrabber::closeReader" );

	if( videoReader ) {
		channel->CloseVideoReader();
		videoReader = 0;

		ancaInfo->setInt( VG_STATE, VG_CLOSED );
	}
}

void VideoGrabber::goSleep()
{
	PTRACE( 6, "VideoGrabber::goSleep" );

	Suspend(true);
}

void VideoGrabber::close()
{
	PWaitAndSignal lock ( mutex );
	run = false;
	if( IsSuspended() ) Resume();
}

void VideoGrabber::stopPreview()
{
	PWaitAndSignal locker( mutex );
	if( state != SPreview ) return;

	PTRACE( 6, "VideoGrabber::stopSPreview" );

	preparing = true;
	state = SSleep;
}

void VideoGrabber::startPreview()
{
	PWaitAndSignal locker( mutex );
//	if( state == SPreview ) return;

	PTRACE( 6, "VideoGrabber::startSPreview" );

	state = SPreview;
	preparing = true;
	if( IsSuspended() ) Resume();
}

void VideoGrabber::startCall()
{
	PWaitAndSignal locker( mutex );
//	if( state == SCall ) return;

	PTRACE( 6, "VideoGrabber::startCall" );

	state = SCall;
	preparing = true;
	if( IsSuspended() ) Resume();
}

void VideoGrabber::stopCall()
{
	PWaitAndSignal locker( mutex );
	if( state != SCall ) return;

	PTRACE( 6, "VideoGrabber::stopCall" );

	preparing = true;
	state = SSleep;
}

void VideoGrabber::registerVideoOutputDeviceCreate( VideoGrabber::VideoOutputDeviceCreate *function )
{
	PWaitAndSignal locker( mutex );
	videoOutputDeviceCreate = function;
}

PVideoChannel *VideoGrabber::getVideoChannel()
{
	return channel;
}

void VideoGrabber::configureVideoFrameSize( unsigned in_width, unsigned in_height, unsigned& out_width, unsigned& out_height )
{
	VideoSize videoSize = (VideoSize)ancaConf->readIntEntry( "videoin", "videoSize", V_LARGE );
	bool previewShowCallSize = ancaConf->readBoolEntry( "videoin", "previewShowCallSize", true );

	if( args->HasOption( "videosize" ) )
		videoSize = (args->GetOptionString("videosize") *= "large") ? V_LARGE : V_SMALL;

	if( state == SPreview && !previewShowCallSize ) {
		out_width = in_width;
		out_height = in_height;
	}
	else {
		out_width  = videoSize == V_SMALL ? PVideoDevice::QCIFWidth : PVideoDevice::CIFWidth;
		out_height = videoSize == V_SMALL ? PVideoDevice::QCIFHeight : PVideoDevice::CIFHeight;
	}
}

