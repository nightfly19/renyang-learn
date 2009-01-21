


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

#ifndef _CONFIG_H
#define _CONFIG_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <memory>
#include <qsettings.h>
#include <qstring.h>

#define DRTA_DEFAULT_INPORT 1793
#define DRTA_DEFAULT_OUTPORT 1793
#define DRTA_DEFAULT_INDRIVER 0
#define DRTA_DEFAULT_OUTDRIVER 0
#define DRTA_DEFAULT_PROTOCOL 2 // SCTP
#define DRTA_DEFAULT_THRESHOLD 20
#define DRTA_DEFAULT_TXSTOP 3
#define DRTA_DEFAULT_RINGVOLUME 2
#define DRTA_DEFAULT_ADRSTRETCH 5
#define DRTA_DEFAULT_ADRMINDELAY 30
#define DRTA_DEFAULT_ADRMAXDELAY 100
#define DRTA_DEFAULT_AGCMIN 0
#define DRTA_DEFAULT_AGCMAX 30
#define DRTA_DEFAULT_AGCSTEP 10
#define DRTA_DEFAULT_AGCLEVEL 50
#define DRTA_DEFAULT_MODE 1
#define DRTA_DEFAULT_BITRATEMODE 1
#define DRTA_DEFAULT_QUALITY 5
#define DRTA_DEFAULT_COMPLEXITY 3
#define DRTA_DEFAULT_VBRQUALITY 5
#define DRTA_DEFAULT_ABR 16
#define DRTA_DEFAULT_KEYLEN 256

#define DRTA_DEFAULT_ANSWER false
#define DRTA_DEFAULT_TRAY true
#define DRTA_DEFAULT_WAIT false
#define DRTA_DEFAULT_HIDE false

#define DRTA_DEFAULT_UDP false
#define DRTA_DEFAULT_TCP false
#define DRTA_DEFAULT_SCTP true
#define DRTA_DEFAULT_SCTP_UDP false

#define DRTA_DEFAULT_ADR false
#define DRTA_DEFAULT_AGC false
#define DRTA_DEFAULT_VAD true
#define DRTA_DEFAULT_DTX true
#define DRTA_DEFAULT_CRYPT false
#define DRTA_DEFAULT_RANDOM true
#define DRTA_DEFAULT_SHOWKEY false

#define DRTA_DEFAULT_INTERFACE "default"

#define DRTA_EXT ".drta"
#define DRTA_MAX_NAME_LEN 64
#define DRTA_UDP 0
#define DRTA_TCP 1
#define DRTA_SCTP 2
#define DRTA_SCTP_UDP 3

#define DRTA_NARROW 0
#define DRTA_WIDE 1
#define DRTA_ULTRAWIDE 2

#define MAPLE_STM_AUDIO 0
#define MAPLE_STM_SPECIAL 1
#define MAPLE_STM_MSG 2
#define MAPLE_STM_NEW 3

/* for sctp setting */
#define DRTA_DEF_SCTP_RTO_MAX 10000
#define DRTA_DEF_SCTP_RTO_MIN  1000
#define DRTA_DEF_SCTP_TTL       100

#define DRTA_DEF_SCTP_ALLOW_AUTO true
#define DRTA_DEF_SCTP_ERR_THR      10
#define DRTA_USE_SCTP_POLICY_RP	    1
#define DRTA_USE_SCTP_POLICY_POLL	    2
#define DRTA_USE_SCTP_POLICY_MUTE	3
#define DRTA_DEF_SCTP_HO_POLICY	    DRTA_USE_SCTP_POLICY_POLL


/**
 * This class is implemented as a Singleton and provides a global interface to 
 * configuration data. It is derived from QSettings, so see the Qt documentation
 * for specific member functions.
 * The constructor initializes everything with default values in case there is 
 * no item found in the configuration file.
 */

class Config : public QSettings {
public:
        /**
	 * Only possible access to Singleton
	 */
	static Config& instance();
	~Config() {};
private:
	typedef std::auto_ptr<Config> ConfigPtr;
	Config();
	void setDefault(const QString& key, const char* value);
	void setDefault(const QString& key, const int value);
	void setDefault(const QString& key, bool value);
};

// for Configure
#define HAVE_LIBASOUND 1
//#define HAVE_LIBJACK 0

#endif

