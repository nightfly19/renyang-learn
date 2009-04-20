


/*
 *  IHU -- I Hear U, easy VoIP application using Speex and Qt
 *
 *  Copyright (C) 2003-2005 M. Trotta - <mrotta@users.sourceforge.net>
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

/*
 *  MOD -- Messenger On-the-Drive
 *
 *  Copyright (C) 2006 Kai-Fong Chou - <mod.mapleelpam at gmail.com>
 *
 *  http://mod.0rz.net
 *  http://mod.sf.net
 *
 */

#include "Config.h"
#include <qdir.h>

Config& Config::instance()
{
	static ConfigPtr i(new Config);
	return *i;
}

Config::Config()
{
	setDefault("/drta/general/name", "Dr.Ta");
	setDefault("/drta/general/dumpin", "");
	setDefault("/drta/general/dumpout", "");
	setDefault("/drta/general/answer", DRTA_DEFAULT_ANSWER);
	setDefault("/drta/general/tray", DRTA_DEFAULT_TRAY);
	setDefault("/drta/general/autowait", DRTA_DEFAULT_WAIT);
	setDefault("/drta/general/autohide", DRTA_DEFAULT_HIDE);
	setDefault("/drta/host/default","");
	setDefault("/drta/net/udp", DRTA_DEFAULT_UDP);
	setDefault("/drta/net/tcp", DRTA_DEFAULT_TCP);
	setDefault("/drta/net/stcp", DRTA_DEFAULT_SCTP);
	setDefault("/drta/net/stcp_UDP", DRTA_DEFAULT_SCTP_UDP);
	setDefault("/drta/net/protocol", DRTA_DEFAULT_PROTOCOL);
	setDefault("/drta/net/inport",DRTA_DEFAULT_INPORT);
	setDefault("/drta/net/outport",DRTA_DEFAULT_OUTPORT);
	setDefault("/drta/sound/inputdriver", DRTA_DEFAULT_INDRIVER);
	setDefault("/drta/sound/outputdriver", DRTA_DEFAULT_OUTDRIVER);
	setDefault("/drta/sound/inputinterface", DRTA_DEFAULT_INTERFACE);
	setDefault("/drta/sound/outputinterface", DRTA_DEFAULT_INTERFACE);
	setDefault("/drta/sound/threshold", DRTA_DEFAULT_THRESHOLD);
	setDefault("/drta/sound/txstop", DRTA_DEFAULT_TXSTOP);
	setDefault("/drta/sound/ringvolume", DRTA_DEFAULT_RINGVOLUME);
	setDefault("/drta/sound/adr", DRTA_DEFAULT_ADR);
	setDefault("/drta/sound/adrstretch", DRTA_DEFAULT_ADRSTRETCH);
	setDefault("/drta/sound/adrmindelay", DRTA_DEFAULT_ADRMINDELAY);
	setDefault("/drta/sound/adrmaxdelay", DRTA_DEFAULT_ADRMAXDELAY);
	setDefault("/drta/sound/agc", DRTA_DEFAULT_AGC);
	setDefault("/drta/sound/agcmin", DRTA_DEFAULT_AGCMIN);
	setDefault("/drta/sound/agcmax", DRTA_DEFAULT_AGCMAX);
	setDefault("/drta/sound/agcstep", DRTA_DEFAULT_AGCSTEP);
	setDefault("/drta/sound/agclevel", DRTA_DEFAULT_AGCLEVEL);
	setDefault("/drta/speex/mode", DRTA_DEFAULT_MODE);
	setDefault("/drta/speex/bitratemode", DRTA_DEFAULT_BITRATEMODE);
	setDefault("/drta/speex/quality", DRTA_DEFAULT_QUALITY);
	setDefault("/drta/speex/complexity", DRTA_DEFAULT_COMPLEXITY);
	setDefault("/drta/speex/vbrquality", DRTA_DEFAULT_VBRQUALITY);
	setDefault("/drta/speex/abr", DRTA_DEFAULT_ABR);
	setDefault("/drta/speex/vad", DRTA_DEFAULT_VAD);
	setDefault("/drta/speex/dtx", DRTA_DEFAULT_DTX);
	setDefault("/drta/security/crypt", DRTA_DEFAULT_CRYPT);
	setDefault("/drta/security/random", DRTA_DEFAULT_RANDOM);
	setDefault("/drta/security/showkey", DRTA_DEFAULT_SHOWKEY);
	setDefault("/drta/security/keylen", DRTA_DEFAULT_KEYLEN);
	setDefault("/drta/security/logfile","");

	setDefault("/drta/sctp/rto_max",DRTA_DEF_SCTP_RTO_MAX);
	setDefault("/drta/sctp/rto_min",DRTA_DEF_SCTP_RTO_MIN);
	setDefault("/drta/sctp/ttl",DRTA_DEF_SCTP_TTL);
	setDefault("/drta/sctp/allow_auto",DRTA_DEF_SCTP_ALLOW_AUTO);
	setDefault("/drta/sctp/err_thr",DRTA_DEF_SCTP_ERR_THR);
	setDefault("/drta/sctp/ho_policy",DRTA_DEF_SCTP_HO_POLICY);
}

// renyang - 透過再重新包裝,把使得所有的在指定預設值時,均可以使用相同的函數名稱
// renyang - 在使用read*Entry()時請注意, 當讀取失敗時, 會用value來直接設定所要用的value值
void Config::setDefault(const QString& key, const char* value)
{
	writeEntry(key,readEntry(key,(const QString)value));
}

void Config::setDefault(const QString& key, const int value)
{
	writeEntry(key,readNumEntry(key,value));
}

void Config::setDefault(const QString& key, bool value)
{
	writeEntry(key,readBoolEntry(key,value));
}
