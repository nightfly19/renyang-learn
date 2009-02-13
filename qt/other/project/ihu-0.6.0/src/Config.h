/*
 *  IHU -- I Hear U, easy VoIP application using Speex and Qt
 *
 *  Copyright (C) 2003-2008 Matteo Trotta - <mrotta@users.sourceforge.net>
 *
 *  http://ihu.sourceforge.net
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111, USA.
 */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qstring.h>

#define IHU_DEFAULT_CONFIG_FILE ".ihu.xml"

#define IHU_DEFAULT_MAXCALLS 5
#define IHU_DEFAULT_MAXHOSTS 20
#define IHU_DEFAULT_INPORT 1793
#define IHU_DEFAULT_OUTPORT 1793
#define IHU_DEFAULT_INDRIVER 0
#define IHU_DEFAULT_PROTOCOL 0
#define IHU_DEFAULT_THRESHOLD -96
#define IHU_DEFAULT_TXSTOP 3
#define IHU_DEFAULT_PREPACKETS 5
#define IHU_DEFAULT_RINGVOLUME 2
#define IHU_DEFAULT_ADRSTRETCH 5
#define IHU_DEFAULT_ADRMINDELAY 0
#define IHU_DEFAULT_ADRMAXDELAY 150
#define IHU_DEFAULT_AGCSTEP 20
#define IHU_DEFAULT_AGCLEVEL -9
#define IHU_DEFAULT_MODE 2
#define IHU_DEFAULT_BITRATEMODE 1
#define IHU_DEFAULT_QUALITY 5
#define IHU_DEFAULT_COMPLEXITY 3
#define IHU_DEFAULT_VBRQUALITY 5
#define IHU_DEFAULT_ABR 16
#define IHU_DEFAULT_KEYLEN 256

#define IHU_DEFAULT_ANSWER false
#define IHU_DEFAULT_TRAY true
#define IHU_DEFAULT_WAIT false
#define IHU_DEFAULT_HIDE false
#define IHU_DEFAULT_UDP true
#define IHU_DEFAULT_TCP true
#define IHU_DEFAULT_ADR false
#define IHU_DEFAULT_AGC false
#define IHU_DEFAULT_AGC_HW true
#define IHU_DEFAULT_AGC_SW false
#define IHU_DEFAULT_VAD true
#define IHU_DEFAULT_DTX true
#define IHU_DEFAULT_CRYPT false
#define IHU_DEFAULT_RANDOM true
#define IHU_DEFAULT_SHOWKEY false

#define IHU_DEFAULT_AGC_CONTROL "Capture"
#define IHU_DEFAULT_INTERFACE "default"

#define IHU_EXT ".ihu"
#define IHU_SPX_EXT ".spx"
#define IHU_MAX_NAME_LEN 64
#define IHU_UDP 0
#define IHU_TCP 1

#define IHU_NARROW 0
#define IHU_WIDE 1
#define IHU_ULTRAWIDE 2

#define IHU_MAX_PASSLEN 56

#define IHU_MAX_CALLS 20
#define IHU_MAX_HOSTS 50

class Config
{
public:
	Config(const char *);
	~Config();

	QString fileName;
	
	QString getFileName();

	bool readConfig();
	void writeConfig();
	void writeConfig(QString);
	void setDefault();
	bool checkConfig();

	QString getHost(int);
	void setHost(int, QString);

	QString getMyName();
	bool getTrayIcon();
	bool getAutoAnswer();
	bool getAutoWait();
	bool getAutoHide();
	int getMaxCalls();
	int getMaxHosts();
	void setMyName(QString);
	void setTrayIcon(bool);
	void setAutoAnswer(bool);
	void setAutoWait(bool);
	void setAutoHide(bool);
	void setMaxCalls(int);
	void setMaxHosts(int);
	
	bool getUDP();
	bool getTCP();
	int getProtocol();
	int getInPort();
	int getOutPort();
	void setUDP(bool);
	void setTCP(bool);
	void setProtocol(int);
	void setInPort(int);
	void setOutPort(int);

	int getInputDriver();
	QString getInputInterface();
	QString getOutputInterface();
	int getThreshold();
	int getTXStop();
	int getPrePackets();
	int getRingVolume();
	void setInputDriver(int);
	void setInputInterface(QString);
	void setOutputInterface(QString);
	void setThreshold(int);
	void setTXStop(int);
	void setPrePackets(int);
	void setRingVolume(int);

	int getSpeexMode();
	int getBitrateMode();
	int getQuality();
	int getComplexity();
	int getVBRQuality();
	int getABR();
	bool getVAD();
	bool getDTX();
	void setSpeexMode(int);
	void setBitrateMode(int);
	void setQuality(int);
	void setComplexity(int);
	void setVBRQuality(int);
	void setABR(int);
	void setVAD(bool);
	void setDTX(bool);

	bool getADR();
	int getADRStretch();
	int getADRMinDelay();
	int getADRMaxDelay();
	bool getAGC();
	bool getAGCHw();
	bool getAGCSw();
	int getAGCStep();
	int getAGCLevel();
	QString getAGCControl();
	void setADR(bool);
	void setADRStretch(int);
	void setADRMinDelay(int);
	void setADRMaxDelay(int);
	void setAGC(bool);
	void setAGCHw(bool);
	void setAGCSw(bool);
	void setAGCStep(int);
	void setAGCLevel(int);
	void setAGCControl(QString);

	bool getCrypt();
	bool getRandom();
	bool getShowKey();
	int getKeyLen();
	QString getPasswd();
	QString getLogFile();
	void setCrypt(bool);
	void setRandom(bool);
	void setShowKey(bool);
	void setKeyLen(int);
	void setPasswd(QString);
	void setLogFile(QString);

	QString* getHosts();
	void addHost(QString);
	void clearHosts();

private:
	QString hosts[IHU_MAX_HOSTS];

	QString myName;
	bool trayIcon;
	bool autoAnswer;
	bool autoWait;
	bool autoHide;
	int maxCalls;
	int maxHosts;

	bool udp;
	bool tcp;
	int protocol;
	int inPort;
	int outPort;

	int inputDriver;
	QString inputInterface;
	QString outputInterface;
	int threshold;
	int txStop;
	int prePackets;
	int ringVolume;

	int speexMode;
	int bitrateMode;
	int quality;
	int complexity;
	int vbrQuality;
	int abr;
	bool vad;
	bool dtx;

	bool adr;
	int adrStretch;
	int adrMinDelay;
	int adrMaxDelay;
	bool agc;
	bool agcHw;
	bool agcSw;
	int agcStep;
	int agcLevel;
	QString agcControl;

	bool crypt;
	bool random;
	bool showKey;
	int keyLen;
	QString passwd;
	QString logFile;
};

#endif

