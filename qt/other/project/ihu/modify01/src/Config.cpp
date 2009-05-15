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

#include "Error.h"
#include "Config.h"

#include <stdlib.h>

#include <qdom.h>
#include <qfile.h>
#include <qstring.h>
#include <qtextstream.h>

// renyang - Configure的建構子
Config::Config(const char *fname)
{
	// renyang - 設定configure檔案在儲在哪一個檔案中
	// renyang - 預設是在家目錄的.ihu.xml檔案中
	fileName = QString(IHU_DEFAULT_CONFIG_FILE);
	setDefault();
	// renyang - fname預設是NULL
	// renyang - 表示說若沒有指定的話, 那我預設的configure檔會存放在$(HOME)/.ihu.xml檔案中
	if (fname != NULL)
		fileName = QString(fname);
	else
	{
		// renyang - 取得環境變數HOME
		char *ptr = getenv("HOME");
		if (ptr != NULL)
		{
			// renyang - 把整個檔名整合起來, 預設的結果會是$(HOME)/.ihu.xml
			fileName = QString("%1/%2").arg(ptr).arg(IHU_DEFAULT_CONFIG_FILE);
		}
	}
	// renyang - 讀取檔案錯誤, 使用程式預設值
	if (!readConfig())
	{
		qWarning(QString("Using default settings..."));
		setDefault();
	}
}

Config::~Config()
{
}

// renyang - 取得configure檔的檔名
QString Config::getFileName()
{
	return fileName;
}

// renyang - 讀取configure檔案
bool Config::readConfig()
{
	// renyang - 應該是記錄是否有讀取檔案成功, 若有一次成功, 就把它設定為true
	bool ret = false;
	// renyang - Creates a document and sets the name of the document type to name.
	// renyang - 這一個類別應該就是要把由configure檔案讀取出來的放到這一個類別中@@@
	QDomDocument doc( "IhuConfigXML" );
	// renyang - 設定要執行configure檔案(預設在$(HOME)/.ihu.xml)
	QFile confFile( fileName );
	// renyang - 開啟檔案, 並且只有讀取的權限
	if( confFile.open( IO_ReadOnly ) )
	{
		// renyang - This function reads the XML document from the IO device dev.
		if( doc.setContent( &confFile ) )
		{
			// renyang - Returns the root element of the document
			QDomElement root = doc.documentElement();
			// renyang - Returns the tag name of this element
			// renyang - 若最上層的tag是ihu的話, 那表示格式上沒有錯
			if( root.tagName() == "ihu" )
			{
				// renyang - 取得最上層的第一個child, 若沒有child的話, 則會回傳Null Node
				QDomNode n = root.firstChild();
				while( !n.isNull() )
				{
					// renyang - Converts a QDomNode into a QDomElement. If the node is not an element the returned object will be null
					QDomElement e = n.toElement();
					if( !e.isNull() )
					{
						// renyang - 再由目前的QDomElement取得目前的tag Name
						QString tagName = e.tagName();
						if (tagName == "general")
						{
							// renyang - 回傳名為"myName"的屬性, 若沒有叫"myName"的屬性的話
							// renyang - 則直接回傳後面的設定值
							myName = e.attribute( "myName", QString("") );
							trayIcon = (bool) e.attribute( "trayIcon", QString("%1").arg(IHU_DEFAULT_TRAY)).toInt();
							autoWait = (bool) e.attribute( "autoWait", QString("%1").arg(IHU_DEFAULT_WAIT) ).toInt();
							autoAnswer = (bool) e.attribute( "autoAnswer", QString("%1").arg(IHU_DEFAULT_ANSWER) ).toInt();
							autoHide = (bool) e.attribute( "autoHide", QString("%1").arg(IHU_DEFAULT_HIDE) ).toInt();
							maxCalls = e.attribute( "maxCalls", QString("%1").arg(IHU_DEFAULT_MAXCALLS) ).toInt();
							maxHosts = e.attribute( "maxHosts", QString("%1").arg(IHU_DEFAULT_MAXHOSTS) ).toInt();
						}
						else if (tagName == "network")
						{
							udp = (bool) e.attribute( "udp", QString("%1").arg(IHU_DEFAULT_UDP)).toInt();
							tcp = (bool) e.attribute( "tcp", QString("%1").arg(IHU_DEFAULT_TCP)).toInt();
							protocol = e.attribute( "protocol", QString("%1").arg(IHU_DEFAULT_PROTOCOL) ).toInt();
							inPort = e.attribute( "inPort", QString("%1").arg(IHU_DEFAULT_INPORT) ).toInt();
							outPort = e.attribute( "outPort", QString("%1").arg(IHU_DEFAULT_OUTPORT) ).toInt();
						}
						else if (tagName == "sound")
						{
							inputDriver = e.attribute( "inputDriver", QString("%1").arg(IHU_DEFAULT_INDRIVER)).toInt();
							inputInterface = e.attribute( "inputInterface", QString(IHU_DEFAULT_INTERFACE));
							outputInterface = e.attribute( "outputInterface", QString(IHU_DEFAULT_INTERFACE));
							threshold = e.attribute( "threshold", QString("%1").arg(IHU_DEFAULT_THRESHOLD) ).toInt();
							txStop = e.attribute( "txStop", QString("%1").arg(IHU_DEFAULT_TXSTOP) ).toInt();
							prePackets = e.attribute( "prePackets", QString("%1").arg(IHU_DEFAULT_PREPACKETS) ).toInt();
							ringVolume = e.attribute( "ringVolume", QString("%1").arg(IHU_DEFAULT_RINGVOLUME) ).toInt();
						}
						else if (tagName == "encoder")
						{
							speexMode = e.attribute( "speexMode", QString("%1").arg(IHU_DEFAULT_MODE)).toInt();
							bitrateMode = e.attribute( "bitrateMode", QString("%1").arg(IHU_DEFAULT_BITRATEMODE)).toInt();
							quality = e.attribute( "quality", QString("%1").arg(IHU_DEFAULT_QUALITY) ).toInt();
							complexity = e.attribute( "complexity", QString("%1").arg(IHU_DEFAULT_COMPLEXITY) ).toInt();
							vbrQuality = e.attribute( "vbrQuality", QString("%1").arg(IHU_DEFAULT_VBRQUALITY) ).toInt();
							abr = e.attribute( "abr", QString("%1").arg(IHU_DEFAULT_ABR) ).toInt();
							vad = (bool) e.attribute( "vad", QString("%1").arg(IHU_DEFAULT_VAD) ).toInt();
							dtx = (bool) e.attribute( "dtx", QString("%1").arg(IHU_DEFAULT_DTX) ).toInt();
						}
						else if (tagName == "options")
						{
							adr = (bool) e.attribute( "adr", QString("%1").arg(IHU_DEFAULT_ADR)).toInt();
							adrStretch = e.attribute( "adrStretch", QString("%1").arg(IHU_DEFAULT_ADRSTRETCH)).toInt();
							adrMinDelay = e.attribute( "adrMinDelay", QString("%1").arg(IHU_DEFAULT_ADRMINDELAY) ).toInt();
							adrMaxDelay = e.attribute( "adrMaxDelay", QString("%1").arg(IHU_DEFAULT_ADRMAXDELAY) ).toInt();
							agc = (bool) e.attribute( "agc", QString("%1").arg(IHU_DEFAULT_AGC) ).toInt();
							agcHw = (bool) e.attribute( "agcHw", QString("%1").arg(IHU_DEFAULT_AGC_HW) ).toInt();
							agcSw = (bool) e.attribute( "agcSw", QString("%1").arg(IHU_DEFAULT_AGC_SW) ).toInt();
							agcStep = e.attribute( "agcStep", QString("%1").arg(IHU_DEFAULT_AGCSTEP) ).toInt();
							agcLevel = e.attribute( "agcLevel", QString("%1").arg(IHU_DEFAULT_AGCLEVEL) ).toInt();
							agcControl = e.attribute( "agcControl", QString("%1").arg(IHU_DEFAULT_AGC_CONTROL) );
						}
						else if (tagName == "security")
						{
							crypt = (bool) e.attribute( "crypt", QString("%1").arg(IHU_DEFAULT_CRYPT)).toInt();
							random = (bool) e.attribute( "random", QString("%1").arg(IHU_DEFAULT_RANDOM)).toInt();
							showKey = (bool) e.attribute( "showKey", QString("%1").arg(IHU_DEFAULT_SHOWKEY) ).toInt();
							keyLen = e.attribute( "keyLen", QString("%1").arg(IHU_DEFAULT_KEYLEN) ).toInt();
							passwd = e.attribute( "passwd", QString(""));
							logFile = e.attribute( "logFile", QString(""));
						}
						else if (tagName == "host")
						{
							for (int i=0; i<IHU_MAX_HOSTS; i++)
								hosts[i] = e.attribute(QString("h%1").arg(i), "");
						}
					}
					// renyang - 會跳到下一個同一階層的tag
					n = n.nextSibling();
				}
				// renyang - 當有一次執行到此, 則表示讀取configure檔案有成功
				ret = true;
			}
			else
			{
				qWarning(QString("Error: %1 is not a valid IHU config file").arg(fileName));
			}
		}
		else
		{
			qWarning(QString("Error: an error occurred while parsing %1").arg(fileName));
		}
		// renyang - 關閉開啟的檔案
		confFile.close();
	}
	else
	{
		qWarning(QString("Error: can't read config file %1 (%2)").arg(fileName).arg(confFile.errorString()));
	}
	return ret;
}

// renyang - 寫入configure到configure檔案中
void Config::writeConfig()
{
	writeConfig(fileName);
}

void Config::writeConfig(QString fname)
{
	QDomDocument doc( "IhuConfigXML" );
	QDomElement root = doc.createElement( "ihu" );
	doc.appendChild( root );

	QDomElement general = doc.createElement( "general" );
	
	general.setAttribute( "myName", myName );
	general.setAttribute( "trayIcon", trayIcon );
	general.setAttribute( "autoWait", autoWait );
	general.setAttribute( "autoAnswer", autoAnswer );
	general.setAttribute( "autoHide", autoHide );
	general.setAttribute( "maxCalls", maxCalls );
	general.setAttribute( "maxHosts", maxHosts );

	QDomElement network = doc.createElement( "network" );
	
	network.setAttribute( "udp", udp );
	network.setAttribute( "tcp", tcp );
	network.setAttribute( "protocol", protocol );
	network.setAttribute( "inPort", inPort );
	network.setAttribute( "outPort", outPort );

	QDomElement sound = doc.createElement( "sound" );

	sound.setAttribute( "inputDriver", inputDriver );
	sound.setAttribute( "inputInterface", inputInterface );
	sound.setAttribute( "outputInterface", outputInterface );
	sound.setAttribute( "threshold", threshold );
	sound.setAttribute( "txStop", txStop );
	sound.setAttribute( "prePackets", prePackets );
	sound.setAttribute( "ringVolume", ringVolume );

	QDomElement encoder = doc.createElement( "encoder" );

	encoder.setAttribute( "speexMode", speexMode );
	encoder.setAttribute( "bitrateMode", bitrateMode );
	encoder.setAttribute( "quality", quality );
	encoder.setAttribute( "complexity", complexity );
	encoder.setAttribute( "vbrQuality", vbrQuality );
	encoder.setAttribute( "abr", abr );
	encoder.setAttribute( "vad", vad );
	encoder.setAttribute( "dtx", dtx );

	QDomElement options = doc.createElement( "options" );

	options.setAttribute( "adr", adr );
	options.setAttribute( "adrStretch", adrStretch );
	options.setAttribute( "adrMinDelay", adrMinDelay );
	options.setAttribute( "adrMaxDelay", adrMaxDelay );
	options.setAttribute( "agc", agc );
	options.setAttribute( "agcHw", agcHw );
	options.setAttribute( "agcSw", agcSw );
	options.setAttribute( "agcStep", agcStep );
	options.setAttribute( "agcLevel", agcLevel );
	options.setAttribute( "agcControl", agcControl );

	QDomElement security = doc.createElement( "security" );

	security.setAttribute( "crypt", crypt );
	security.setAttribute( "random", random );
	security.setAttribute( "showKey", showKey );
	security.setAttribute( "keyLen", keyLen );
	security.setAttribute( "passwd", passwd );
	security.setAttribute( "logFile", logFile );

	QDomElement host = doc.createElement( "host" );

	for (int i=0; i<maxHosts; i++)
		host.setAttribute(QString("h%1").arg(i), hosts[i]);

	root.appendChild(general);
	root.appendChild(network);
	root.appendChild(sound);
	root.appendChild(encoder);
	root.appendChild(options);
	root.appendChild(security);
	root.appendChild(host);

	QFile confFile( fname );
	if( confFile.open( IO_WriteOnly ) )
	{
		QTextStream ts( &confFile );
		ts << doc.toString();
		confFile.close();
	}
	else
	{
		throw Error(QString("can't write config file %1 (%2)").arg(fileName).arg(confFile.errorString()));
	}
}

void Config::setDefault()
{
	// renyang - 設定一些相關預設參數
	setMyName(QString(""));
	setTrayIcon(IHU_DEFAULT_TRAY);
	setAutoAnswer(IHU_DEFAULT_ANSWER);
	setAutoWait(IHU_DEFAULT_WAIT);
	setAutoHide(IHU_DEFAULT_HIDE);
	setMaxCalls(IHU_DEFAULT_MAXCALLS);
	setMaxHosts(IHU_DEFAULT_MAXHOSTS);

	// renyang - 網路相關預設值
	setUDP(IHU_DEFAULT_UDP);
	setTCP(IHU_DEFAULT_TCP);
	setProtocol(IHU_DEFAULT_PROTOCOL);
	setInPort(IHU_DEFAULT_INPORT);
	setOutPort(IHU_DEFAULT_OUTPORT);

	// renyang - 設定聲音相關設定
	setInputDriver(IHU_DEFAULT_INDRIVER);
	setInputInterface(IHU_DEFAULT_INTERFACE);
	setOutputInterface(IHU_DEFAULT_INTERFACE);
	setThreshold(IHU_DEFAULT_THRESHOLD);
	setTXStop(IHU_DEFAULT_TXSTOP);
	setPrePackets(IHU_DEFAULT_PREPACKETS);
	setRingVolume(IHU_DEFAULT_RINGVOLUME);

	setSpeexMode(IHU_DEFAULT_MODE);
	setBitrateMode(IHU_DEFAULT_BITRATEMODE);
	setQuality(IHU_DEFAULT_QUALITY);
	setComplexity(IHU_DEFAULT_COMPLEXITY);
	setVBRQuality(IHU_DEFAULT_VBRQUALITY);
	setABR(IHU_DEFAULT_ABR);
	setVAD(IHU_DEFAULT_VAD);
	setDTX(IHU_DEFAULT_DTX);

	setADR(IHU_DEFAULT_ADR);
	setADRStretch(IHU_DEFAULT_ADRSTRETCH);
	setADRMinDelay(IHU_DEFAULT_ADRMINDELAY);
	setADRMaxDelay(IHU_DEFAULT_ADRMAXDELAY);
	setAGC(IHU_DEFAULT_AGC);
	setAGCHw(IHU_DEFAULT_AGC_HW);
	setAGCSw(IHU_DEFAULT_AGC_SW);
	setAGCStep(IHU_DEFAULT_AGCSTEP);
	setAGCLevel(IHU_DEFAULT_AGCLEVEL);
	setAGCControl(IHU_DEFAULT_AGC_CONTROL);

	setCrypt(IHU_DEFAULT_CRYPT);
	setRandom(IHU_DEFAULT_RANDOM);
	setShowKey(IHU_DEFAULT_SHOWKEY);
	setKeyLen(IHU_DEFAULT_KEYLEN);
	setPasswd(QString(""));
	// renyang - 預設是沒有設定記錄檔的位置, 也就是說, 沒有存放記錄檔啦
	setLogFile(QString(""));

	clearHosts();
}

void Config::addHost(QString val)
{
	if (val != hosts[0])
	{
		for (int i=(maxHosts-1); i>0; i--)
			hosts[i] = hosts[i-1];
		hosts[0] = val;
	}
}

void Config::clearHosts()
{
	for (int i=0; i<IHU_MAX_HOSTS; i++)
		hosts[i] = QString("");
}

QString* Config::getHosts()
{
	return hosts;
}

QString Config::getMyName()
{
	return myName;
}

void Config::setMyName(QString val)
{
	myName = val;
}

bool Config::getTrayIcon()
{
	return trayIcon;
}

void Config::setTrayIcon(bool val)
{
	trayIcon = val;
}

bool Config::getAutoAnswer()
{
	return autoAnswer;
}

void Config::setAutoAnswer(bool val)
{
	autoAnswer = val;
}

bool Config::getAutoWait()
{
	return autoWait;
}

void Config::setAutoWait(bool val)
{
	autoWait = val;
}

bool Config::getAutoHide()
{
	return autoHide;
}

void Config::setAutoHide(bool val)
{
	autoHide = val;
}

int Config::getMaxCalls()
{
	return maxCalls;
}

void Config::setMaxCalls(int val)
{
	maxCalls = val;
}

int Config::getMaxHosts()
{
	return maxHosts;
}

void Config::setMaxHosts(int val)
{
	maxHosts = val;
}

bool Config::getUDP()
{
	return udp;
}

void Config::setUDP(bool val)
{
	udp = val;
}

bool Config::getTCP()
{
	return tcp;
}

void Config::setTCP(bool val)
{
	tcp = val;
}

int Config::getProtocol()
{
	return protocol;
}

void Config::setProtocol(int val)
{
	protocol = val;
}

int Config::getInPort()
{
	return inPort;
}

void Config::setInPort(int val)
{
	inPort = val;
}

int Config::getOutPort()
{
	return outPort;
}

void Config::setOutPort(int val)
{
	outPort = val;
}

int Config::getInputDriver()
{
	return inputDriver;
}

void Config::setInputDriver(int val)
{
	inputDriver = val;
}

QString Config::getInputInterface()
{
	return inputInterface;
}

void Config::setInputInterface(QString val)
{
	inputInterface = val;
}

QString Config::getOutputInterface()
{
	return outputInterface;
}

void Config::setOutputInterface(QString val)
{
	outputInterface = val;
}

int Config::getThreshold()
{
	return threshold;
}

void Config::setThreshold(int val)
{
	threshold = val;
}

int Config::getTXStop()
{
	return txStop;
}

void Config::setTXStop(int val)
{
	txStop = val;
}

int Config::getPrePackets()
{
	return prePackets;
}

void Config::setPrePackets(int val)
{
	prePackets = val;
}

int Config::getRingVolume()
{
	return ringVolume;
}

void Config::setRingVolume(int val)
{
	ringVolume = val;
}

int Config::getSpeexMode()
{
	return speexMode;
}

void Config::setSpeexMode(int val)
{
	speexMode = val;
}

int Config::getBitrateMode()
{
	return bitrateMode;
}

void Config::setBitrateMode(int val)
{
	bitrateMode = val;
}

int Config::getQuality()
{
	return quality;
}

void Config::setQuality(int val)
{
	quality = val;
}

int Config::getComplexity()
{
	return complexity;
}

void Config::setComplexity(int val)
{
	complexity = val;
}

int Config::getVBRQuality()
{
	return vbrQuality;
}

void Config::setVBRQuality(int val)
{
	vbrQuality = val;
}

int Config::getABR()
{
	return abr;
}

void Config::setABR(int val)
{
	abr = val;
}

bool Config::getVAD()
{
	return vad;
}

void Config::setVAD(bool val)
{
	vad = val;
}

bool Config::getDTX()
{
	return dtx;
}

void Config::setDTX(bool val)
{
	dtx = val;
}

bool Config::getADR()
{
	return adr;
}

void Config::setADR(bool val)
{
	adr = val;
}

int Config::getADRStretch()
{
	return adrStretch;
}

void Config::setADRStretch(int val)
{
	adrStretch = val;
}

int Config::getADRMinDelay()
{
	return adrMinDelay;
}

void Config::setADRMinDelay(int val)
{
	adrMinDelay = val;
}

int Config::getADRMaxDelay()
{
	return adrMaxDelay;
}

void Config::setADRMaxDelay(int val)
{
	adrMaxDelay = val;
}

bool Config::getAGC()
{
	return agc;
}

void Config::setAGC(bool val)
{
	agc = val;
}

bool Config::getAGCHw()
{
	return agcHw;
}

void Config::setAGCHw(bool val)
{
	agcHw = val;
}

bool Config::getAGCSw()
{
	return agcSw;
}

void Config::setAGCSw(bool val)
{
	agcSw = val;
}

int Config::getAGCStep()
{
	return agcStep;
}

void Config::setAGCStep(int val)
{
	agcStep = val;
}

int Config::getAGCLevel()
{
	return agcLevel;
}

void Config::setAGCLevel(int val)
{
	agcLevel = val;
}

QString Config::getAGCControl()
{
	return agcControl;
}
void Config::setAGCControl(QString val)
{
	agcControl = val;
}

bool Config::getCrypt()
{
	return crypt;
}

void Config::setCrypt(bool val)
{
	crypt = val;
}

bool Config::getRandom()
{
	return random;
}

void Config::setRandom(bool val)
{
	random = val;
}

bool Config::getShowKey()
{
	return showKey;
}

void Config::setShowKey(bool val)
{
	showKey = val;
}

int Config::getKeyLen()
{
	return keyLen;
}

void Config::setKeyLen(int val)
{
	keyLen = val;
}

QString Config::getPasswd()
{
	return passwd;
}

void Config::setPasswd(QString val)
{
	if (val.length() > IHU_MAX_PASSLEN)
		val.truncate(IHU_MAX_PASSLEN);
	passwd = val;
}

QString Config::getLogFile()
{
	return logFile;
}

void Config::setLogFile(QString val)
{
	logFile = val;
}

// renyang - 確認所有的數值是否正確
bool Config::checkConfig()
{
	if ((maxCalls < 0) || (maxCalls > IHU_MAX_CALLS))
		return false;
	if ((maxHosts < 0) || (maxHosts > IHU_MAX_HOSTS))
		return false;

	// renyang - 網路的部分, 最後可能要增加sctp
	if (!udp && !tcp)
		return false;
	if ((protocol < 0) || (protocol > 1))
		return false;
	if ((inPort < 0) || (inPort > 32768))
		return false;
	if ((outPort < 0) || (outPort > 32768))
		return false;

	// renyang - 音量；所需要先準備的封包
	if ((threshold < -96) || (threshold > 0))
		return false;
	if ((txStop < 0) || (txStop > 30))
		return false;
	if ((prePackets < 1) || (prePackets > 20))
		return false;
	if ((ringVolume < 0) || (ringVolume > 3))
		return false;
	if ((prePackets < 0) || (prePackets > 20))
		return false;

	// renyang - 語音模式
	if ((speexMode < 0) || (speexMode > 2))
		return false;
	if ((bitrateMode < 0) || (bitrateMode > 2))
		return false;
	if ((quality < 0) || (quality > 10))
		return false;
	if ((complexity < 1) || (complexity > 10))
		return false;
	if ((vbrQuality < 0) || (vbrQuality > 10))
		return false;
	if ((abr < 2) || (abr > 44))
		return false;

	// renyang - ???
	if ((adrStretch < 1) || (adrStretch > 50))
		return false;
	if ((adrMinDelay < 0) || (adrMinDelay > 9999))
		return false;
	if ((adrMaxDelay < 50) || (adrMaxDelay > 9999))
		return false;

	if (!agcHw && !agcSw)
		return false;
	if ((agcStep < 1) || (agcStep > 100))
		return false;
	if ((agcLevel < -96) || (agcLevel > 0))
		return false;

	if ((keyLen < 8) || (keyLen > 448))
		return false;
	if (passwd.length() > IHU_MAX_PASSLEN)
		passwd.truncate(IHU_MAX_PASSLEN);

	return true;
}
