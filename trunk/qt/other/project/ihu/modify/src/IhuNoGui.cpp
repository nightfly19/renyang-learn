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

#include <stdlib.h>

#include <qstring.h>
#include <qtimer.h>
 
#include "Error.h"
#include "IhuNoGui.hpp"

#define STAT_TIME 1000

IhuNoGui::IhuNoGui(Config& ihucfg) : ihuconfig(ihucfg)
{
}

IhuNoGui::~IhuNoGui()
{
	delete phone;
	delete fileplayer;
	delete logger;
}

void IhuNoGui::initIhu()
{
	try
	{
		if (!ihuconfig.checkConfig())
		{
			qWarning(QString("\nERROR: %1 contains invalid settings!\nPlease remove the file and restart IHU to restore settings.\nIHU will start with default settings now...\n").arg(ihuconfig.getFileName()));
			ihuconfig.setDefault();
		}

		phone = new Phone(ihuconfig.getMaxCalls());
		fileplayer = new FilePlayer();
		logger = new Logger();
		timer = new QTimer(this);

		connect( fileplayer, SIGNAL(keyRequest()), this, SLOT(keyRequest()) );
		connect( fileplayer, SIGNAL(finish()), this, SLOT(stopFile()) );
		connect( fileplayer, SIGNAL(warning(QString)), this, SLOT(message(QString)) );
		connect( fileplayer, SIGNAL(error(QString)), this, SLOT(abortAll(QString)) );
	
		connect( phone, SIGNAL(receivedCallSignal(int)), this, SLOT(receivedCall(int)) );
		connect( phone, SIGNAL(connectedCallSignal(int)), this, SLOT(connectedCall(int)) );
		connect( phone, SIGNAL(cancelCallSignal(int)), this, SLOT(cancelCall(int)) );
		connect( phone, SIGNAL(abortSignal(QString)), this, SLOT(abortAll(QString)) );
		connect( phone, SIGNAL(abortCallSignal(int, QString)), this, SLOT(abortCall(int, QString)) );
		connect( phone, SIGNAL(messageSignal(int, QString)), this, SLOT(message(int, QString)) );
		connect( phone, SIGNAL(warningSignal(QString)), this, SLOT(message(QString)) );
		connect( phone, SIGNAL(newKeySignal(int, QString)), this, SLOT(receivedNewKey(int, QString)) );
		connect( phone, SIGNAL(cryptedSignal(int)), this, SLOT(changeKey(int)) );

		connect( timer, SIGNAL(timeout()), this, SLOT(statistics()) );

		applySettings();

		phone->createCall();

		timer->start(STAT_TIME, false);
	}
	catch (Error e)
	{
		abortAll(e.getText());
	}
}

void IhuNoGui::applySettings()
{
	phone->setMyName(ihuconfig.getMyName());
	
	int abr = ihuconfig.getABR()*1000;
	int vbr = ihuconfig.getBitrateMode();
	switch (vbr)
	{
		case 0:
				abr = 0;
				break;
		case 1:
				abr = 0;
				break;
		case 2:
				vbr = 0;
				break;
	}
	float vbrquality = (float) ihuconfig.getVBRQuality();
	int dtx = ihuconfig.getDTX();
	int th = -96;
	if (dtx)
		th = ihuconfig.getThreshold();

	phone->setupRecorder(ihuconfig.getInputDriver(), ihuconfig.getInputInterface());
	phone->setupPlayer(ihuconfig.getOutputInterface(), ihuconfig.getPrePackets());
	fileplayer->setupPlayer(ihuconfig.getOutputInterface());
	
	phone->setup(ihuconfig.getSpeexMode(), ihuconfig.getQuality(), abr, vbr, vbrquality, ihuconfig.getComplexity(), ihuconfig.getVAD(), dtx, ihuconfig.getTXStop(), th, ihuconfig.getRingVolume());

	adrRefresh(ihuconfig.getADR());
	agcRefresh(ihuconfig.getAGC());

	try
	{
		logger->enable(ihuconfig.getLogFile());
	}
	catch (Error e)
	{
		warning(e.getText());
	}

	autocrypt = ihuconfig.getCrypt();
	showkey = ihuconfig.getShowKey();
}

void IhuNoGui::waitForCalls()
{
	try
	{
		phone->waitCalls(ihuconfig.getInPort(), ihuconfig.getUDP(), ihuconfig.getTCP());
		message(logger->logStartReceiver());
	}
	catch (Error e)
	{
		abortAll(e.getText());
	}
}

void IhuNoGui::call(QString host)
{
	try {
		if (host.isEmpty())
			throw Error(tr("No host specified!"));
		int callPort = ihuconfig.getOutPort();
		QString callHost = host;
		int tmpInd = host.findRev(':');
		if (tmpInd > 0)
		{
			callHost = host.left(tmpInd);
			callPort = host.right(host.length() - tmpInd - 1).toInt();
		}
		if (autocrypt)
			cryptOn(0);
		phone->call(0, callHost, callPort, ihuconfig.getProtocol());
		message(logger->logOutgoingCall(host, phone->getCallerIp(0)));
	}
	catch (Error e)
	{
		abortAll(e.getText());
	}
}

void IhuNoGui::receivedCall(int id)
{
	try
	{
		message(logger->logReceivedCall(phone->getCallerIp(id), phone->getCallerName(id)));
		if (autocrypt)
			cryptOn(id);
		phone->answerCall(id);
	}
	catch (Error e)
	{
		abortAll(e.getText());
	}
}

void IhuNoGui::connectedCall(int id)
{
	message(logger->logConnectedCall(phone->getCallerIp(id), phone->getCallerName(id)));
}

void IhuNoGui::cancelCall(int id)
{
	message(logger->logStop(phone->getCallerIp(id), phone->getCallRX(id), phone->getCallTX(id)));
}

void IhuNoGui::abortCall(int id, QString text)
{
	message(logger->logAbortedCall(phone->getCallerIp(id), text));
}

void IhuNoGui::message(QString text)
{
	qWarning(QString("Warning: %1").arg(text));
}

void IhuNoGui::message(int id, QString text)
{
	qWarning(logger->log(text));
}

void IhuNoGui::abortAll(QString text)
{
	phone->abortAll();
	qWarning("Error: " + text);
	throw Error(text);
}

void IhuNoGui::playFile(QString name)
{
	if (!name.isEmpty())
	{
		try
		{
			warning("Playing " + name + "...");
			fileplayer->playFile(name);
		}
		catch (Error e)
		{
			abortAll(e.getText());
		}
	}
}

void IhuNoGui::stopFile()
{
	message(tr("End of file"));
	fileplayer->end();
	exit(0);
}

void IhuNoGui::agcRefresh(bool on)
{
	try
	{
		float step = (float) (ihuconfig.getAGCStep()/100.f);
		float level = (float) (ihuconfig.getAGCLevel());
		phone->setupAgc(on, ihuconfig.getAGCHw(), ihuconfig.getAGCHw(), step, level, ihuconfig.getAGCControl());
	}
	catch (Error e)
	{
		warning(e.getText());
	}
}

void IhuNoGui::adrRefresh(bool on)
{
	float mindelay = (float) (ihuconfig.getADRMinDelay()/1000.f);
	float maxdelay = (float) (ihuconfig.getADRMinDelay()/1000.f);
	float stretch = (float) ihuconfig.getADRStretch();
	phone->setupAdr(on, mindelay, maxdelay, stretch);
}

void IhuNoGui::cryptOn()
{
	cryptOn(0);
}

void IhuNoGui::cryptOn(int id)
{
	try
	{
		int len = ihuconfig.getKeyLen()/8;
		phone->enableRandomCrypt(id, len);
		warning("Encryption enabled.");
	}
	catch (Error e)
	{
		abortAll(e.getText());
	}
}

void IhuNoGui::disableIn()
{
	phone->disableRecorder(true);
}

void IhuNoGui::disableOut()
{
	phone->disablePlayer(true);
}

void IhuNoGui::keyRequest()
{
	qWarning("Stream is encrypted, but the decryption key is not available");
	fileplayer->noDecrypt();
}

void IhuNoGui::receivedNewKey(int callId, QString text)
{
	if (ihuconfig.getShowKey())
	{
		QString keyMsg = QString("New decryption key: %1").arg(text);
		message(logger->log(QString("%1 (%2) - %3").arg(phone->getCallerIp(callId)).arg(phone->getCallerName(callId)).arg(keyMsg)));
	}
}

void IhuNoGui::changeKey(int id)
{
	int len = ihuconfig.getKeyLen()/8;
	phone->enableRandomCrypt(id, len);
	message("Encryption key changed successfully.");
}

void IhuNoGui::quit()
{
	phone->endAll();
	phone->stopWaiting();
}

void IhuNoGui::statistics()
{
	phone->getPeak();
}
