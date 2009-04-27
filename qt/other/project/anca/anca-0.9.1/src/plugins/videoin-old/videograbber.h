#ifndef VIDEOGRABBER_H
#define VIDEOGRABBER_H

#include <ptlib.h>

class VideoGrabber: public PThread {
	PCLASSINFO(VideoGrabber, PThread);

public:
	enum VideoSize { V_SMALL, V_LARGE };
	
	VideoGrabber( PConfigArgs *args );
	~VideoGrabber();

	typedef PVideoOutputDevice *VideoOutputDeviceCreate();
	void registerVideoOutputDeviceCreate( VideoOutputDeviceCreate *function );
	
	void Main();

	/**
	 * Close thread
	 */
	void close();
	
	/**
	 * Start (or resume) preview thread.
	 * Asynchronous.
	 */
	void startPreview();
	
	/**
	 * Suspend preview thread
	 * Asynchronous.
	 */
	void stopPreview();
	
	PVideoChannel *getVideoChannel();
	
	void configurationChanged();
	
	/**
	 * Asynchronous.
	 */
	void startCall();
	/**
	 * Asynchronous.
	 */
	void stopCall();

	bool preparing;

protected:
	bool openDevices();
	bool openPlayer();
	bool openReader();
	
	void goSleep();
	void closeDevices();
	void closePlayer();
	void closeReader();

	void configureVideoFrameSize( unsigned in_width, unsigned in_height, unsigned& out_width, unsigned& out_height );
	
	VideoOutputDeviceCreate *videoOutputDeviceCreate;

	PVideoChannel *channel;
	PVideoInputDevice *videoReader;
	PVideoOutputDevice *videoPlayer;

	bool reconfigure;
	bool run;

	enum State {
		SSleep,
		SCall,
		SPreview
	} state;

	PMutex *deviceMutex() { return &mutex; }

	PConfigArgs *args;

private:
	PMutex mutex;

	int bufferSize;
	char *buffer;
	
};
extern VideoGrabber *videoGrabber;

#endif
