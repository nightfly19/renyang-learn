/***************************************************************************
*   Copyright (C) 2004 by Tom Potrusil                                    *
*   tom@matfyz.cz                                                         *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

#include "audioplugin.h"
#include "ancaconf.h"
#include "configdialog.h"

MyAudioPlugin *plugin = 0;

MyAudioPlugin::MyAudioPlugin( PluginInfo *info ): AudioPlugin(info), args(0), widget(0)
{
	plugin = this;
}

void MyAudioPlugin::init( PConfigArgs& args_)
{
	args = &args_;

	ancaConf->installNotify( "audioPlayerDevice", this, SLOT( playerDeviceChanged() ), "audio" );
	ancaConf->installNotify( "audioRecorderDevice", this, SLOT( recorderDeviceChanged() ), "audio" );
}

void MyAudioPlugin::exit()
{
	PAssertNULL(endPoint);

	ancaConf->removeNotify( "audioPlayerDevice", this, SLOT( playerDeviceChanged() ), "audio" );
	ancaConf->removeNotify( "audioRecorderDevice", this, SLOT( recorderDeviceChanged() ), "audio" );

	PStringList codecs;
	for( QValueList<CapabilityItem*>::iterator it = capList.begin(); it != capList.end(); ++it ) {
		CapabilityItem *item = *it;
		if( item->isUsed() ) codecs.AppendString(item->getName().latin1());
	}
	endPoint->RemoveCapabilities(codecs);

	//delete capList items...
	for( QValueList<CapabilityItem*>::iterator it = capList.begin(); it != capList.end(); ++it ) {
		delete *it;
	}

	if( endPoint )
		endPoint->RemoveCapabilities( codecs );
	delete widget;

	plugin = 0;
}

ConfigWidget *MyAudioPlugin::createCodecsConfig( QWidget *parent )
{
	widget = new AudioConfigWidget(endPoint, parent, "Audio config widget");

	createViewItems();

	connect( widget, SIGNAL( apply( const QValueList<CapabilityItem*>& ) ), this, SLOT( updateCapabilities( const QValueList<CapabilityItem*>& ) ) );
	connect( widget, SIGNAL( resetView() ), this, SLOT( resetView() ) );

	return widget;
}

QDialog *MyAudioPlugin::createConfig( QWidget *parent )
{
	return new ConfigDialog( parent, "Audio configure dialog" );
}

void MyAudioPlugin::recorderDeviceChanged()
{
	setSoundDevice( ancaConf->readEntry( "audioRecorderDevice" ), PSoundChannel::Recorder );
}

void MyAudioPlugin::playerDeviceChanged()
{
	setSoundDevice( ancaConf->readEntry( "audioPlayerDevice" ), PSoundChannel::Player );
}

void MyAudioPlugin::setSoundDevices()
{
	PString player = ancaConf->readEntry( "audio", "audioPlayerDevice", (const char *)PSoundChannel::GetDefaultDevice(PSoundChannel::Player) ).latin1();
	PString recorder = ancaConf->readEntry( "audio", "audioRecorderDevice", (const char *)PSoundChannel::GetDefaultDevice(PSoundChannel::Recorder) ).latin1();

	if( args->HasOption("sound") )
		player = recorder = args->GetOptionString("sound");
	if( args->HasOption("sound-in") )
		recorder = args->GetOptionString("sound-in");
	if( args->HasOption("sound-out") )
		player = args->GetOptionString("sound-out");

	if (!setSoundDevice( (const char *)recorder, PSoundChannel::Recorder) )
		PTRACE( 1, "Recording device (" << recorder << ") could not be set" );
	if (!setSoundDevice( (const char *)player, PSoundChannel::Player) )
		PTRACE( 1, "Playing device (" << player << ") could not be set" );

//	cout << "Sound output device: \"" << endPoint->GetSoundChannelPlayDevice() << "\"\n"
//		"Sound  input device: \"" << endPoint->GetSoundChannelRecordDevice() << "\"\n";

}

BOOL MyAudioPlugin::setSoundDevice( const QString& dev, PSoundChannel::Directions dir)
{
	if (dir == PSoundChannel::Player) {
		if (endPoint->SetSoundChannelPlayDevice(dev.latin1()))
		return TRUE;
	}
	else {
		if (endPoint->SetSoundChannelRecordDevice(dev.latin1()))
		return TRUE;
	}
	
//	cout << "Device for " << (dir == PSoundChannel::Player ? "playing" : "recording") << " (\"" << dev.latin1() << "\") must be one of:\n";
//	
//	PStringArray names = PSoundChannel::GetDeviceNames(dir);
//	for (PINDEX i = 0; i < names.GetSize(); i++)
//		cout << "  \"" << names[i] << "\"\n";
	
	return FALSE;
}

void MyAudioPlugin::createViewItems()
{
	// insert viewItems one after another
	CodecViewItem *lastViewItem = 0;
	for( QValueList<CapabilityItem*>::iterator it = capList.begin(); it != capList.end(); ++it )
		lastViewItem = new CodecViewItem(widget, lastViewItem, *it);
}

void MyAudioPlugin::resetView()
{
	configureCapList();
	createViewItems();
}

void MyAudioPlugin::configureCapList()
{
	QStringList disabled = ancaConf->readListEntry("audio", "disabledCapabilities", QStringList() );
	QStringList order = ancaConf->readListEntry("audio", "capabilitiesOrder", QStringList() );

	// enable/disable capabilities according to config
	for( QValueListConstIterator<CapabilityItem*> it = capList.begin(); it != capList.end(); ++it ) {
		CapabilityItem *cap = *it;
		cap->setUsed( !disabled.contains(cap->getCaption() ) );
	}

	// sort capabilities
	if( !order.isEmpty() ) {
		// Let's sort capabilities according to order list,
		// tmp list will contain the ordered capabilities.
		QValueList<CapabilityItem*> tmp;

		// This is stupid algorithm. For each string in the order list search our list of capabilities and
		// append found capability to the tmp list.
		for( QStringList::iterator it = order.begin(); it != order.end(); ++it ) {
			CapabilityItem *item = 0;
			for( QValueList<CapabilityItem*>::iterator cit = capList.begin(); cit != capList.end(); ++cit ) {
				CapabilityItem *i = *cit;
				if( i->getCaption() == *it ) {
					item = i;
					break;
				}
			}
			if( item )
				tmp.append(item);
		}

		// Append the rest of capabilities...
		for( QValueList<CapabilityItem*>::iterator cit = capList.begin(); cit != capList.end(); ++cit ) {
			CapabilityItem *i = *cit;
			if( !tmp.contains(i) )
				tmp.append(i);
		}

		// Good, tmp list now the one we want
		capList = tmp;
	}
}

void MyAudioPlugin::addCapabilities()
{
	PAssertNULL(endPoint);

	setSoundDevices();

	capList.append( new CapabilityItem( QString::null, "SpeexNarrow-8k", "8 Kbps") );
	capList.append( new CapabilityItem( QString::null, "GSM-06.10", "16.5 Kbps" ) );
	capList.append( new CapabilityItem( QString::null, "G.711-uLaw-64k", "64 Kbps" ) );
	capList.append( new CapabilityItem( QString::null, "G.711-ALaw-64k", "64 Kbps" ) );
	capList.append( new CapabilityItem( QString::null, "LPC-10", "3.46 Kbps" ) );

	configureCapList();
	updateCapabilities(capList);
}

void MyAudioPlugin::updateCapabilities( const QValueList<CapabilityItem*>& capList )
{
	PAssertNULL(endPoint);

	//Remove all capabilities, even those that are not now used
	PStringList capsToRemove;
	for( QValueListConstIterator<CapabilityItem*> it = capList.begin(); it != capList.end(); ++it ) {
		CapabilityItem *cap = *it;
		if( !cap->getName().isNull() )
			capsToRemove.AppendString( cap->getName().latin1() );
	}
	endPoint->RemoveCapabilities(capsToRemove);

	QStringList disabled, order;

	//Insert new capabilities...
	for( QValueListConstIterator<CapabilityItem*> it = capList.begin(); it != capList.end(); ++it ) {
		CapabilityItem *capItem = *it;
		if( capItem->isUsed() ) {
			H323AudioCapability *cap = 0;

			if( capItem->getCaption() == "GSM-06.10" ) {
				cap = new H323_GSM0610Capability();
				((H323_GSM0610Capability*)cap)->SetTxFramesInPacket( 4 ); // For GSM 06.10, 1 frame ~ 20 milliseconds
				capItem->setName((const char *)cap->GetFormatName());
			}
			else if( capItem->getCaption() == "SpeexNarrow-8k" ) {
				cap = new SpeexNarrow3AudioCapability();
				((SpeexNarrow3AudioCapability*)cap)->SetTxFramesInPacket( 5 ); // For Speex, 1 frame ~ 20 milliseconds
				capItem->setName((const char *)cap->GetFormatName());
			}
			else if( capItem->getCaption() == "G.711-uLaw-64k" ) {
				cap = new H323_G711Capability( H323_G711Capability::muLaw );
				capItem->setName((const char *)cap->GetFormatName());
			}
			else if( capItem->getCaption() == "G.711-ALaw-64k" ) {
				cap = new H323_G711Capability( H323_G711Capability::ALaw );
				capItem->setName((const char *)cap->GetFormatName());
			}
			else if( capItem->getCaption() == "LPC-10" ) {
				cap = new H323_LPC10Capability( *endPoint );
				capItem->setName((const char *)cap->GetFormatName());
			}

			if( cap )
				endPoint->SetCapability( 0, 0, cap );
		}
		else
			disabled.append(capItem->getCaption());

		order.append(capItem->getCaption());
	}

	ancaConf->writeListEntry( "audio", "disabledCapabilities", disabled );
	ancaConf->writeListEntry( "audio", "capabilitiesOrder", order );
}

PChannel *MyAudioPlugin::getChannel( bool recording )
{
	PSoundChannel *soundChannel = new PSoundChannel();
	PString deviceName;
	if (recording)
		deviceName = endPoint->GetSoundChannelRecordDevice();
	else
		deviceName = endPoint->GetSoundChannelPlayDevice();

	if (soundChannel->Open(deviceName, 
				recording ? PSoundChannel::Recorder : PSoundChannel::Player,
				1, 8000, 16)) {
		PTRACE(3, "Opened sound channel \"" << deviceName
		       << "\" for " << (recording ? "record" : "play")
		       << "ing using " << endPoint->GetSoundChannelBufferDepth()
		       << 'x' << bufferSize << " byte buffers.");
		soundChannel->SetBuffers(bufferSize, endPoint->GetSoundChannelBufferDepth() );
		return soundChannel;
	}

	delete soundChannel;
	return 0;
}

void MyAudioPlugin::prepareForCall()
{}

void MyAudioPlugin::startPreview()
{}

void MyAudioPlugin::stopPreview()
{}

/***************** plugin stuff ********************/

Plugin *createMyAudioPlugin();
Plugin *getMyAudioPlugin();

static PluginInfo pluginInfo = {
                                   Plugin::Audio,
                                   P_CAN_UNLOAD | P_HAS_CONFIG,
                                   "audio",
                                   "Configures audio codecs",
                                   "Tomas Potrusil",
                                   createMyAudioPlugin,
                                   getMyAudioPlugin,
				   5
                               };

Plugin *createMyAudioPlugin()
{
	return new MyAudioPlugin( &pluginInfo );
}

Plugin *getMyAudioPlugin()
{
	return plugin;
}

extern "C" PluginInfo *getPluginInfo()
{
	return & pluginInfo;
}

