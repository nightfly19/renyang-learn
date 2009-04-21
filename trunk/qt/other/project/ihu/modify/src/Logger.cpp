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
#include "Logger.h"
#include <string.h>
#include <errno.h>
#include <qdatetime.h>

Logger::Logger()
{
	logfile = NULL;
}

Logger::~Logger()
{
	end();
}

void Logger::enable(QString file)
{
	end();
	if (!file.isEmpty()) {
		logfile = fopen(file.ascii(), "a");
		if (logfile==NULL)
			throw Error(QString("%1: %2").arg(file).arg(strerror(errno)));
		previous = QString::null;
	}
}

QString Logger::log(QString text)
{
	QString logText;
	if (text != previous) // Avoid to log identical messages several times
	{
		logText = QString("%1 - %2").arg(getTime()).arg(text);
		if (logfile)
		{
			fprintf(logfile, "%s\n", logText.ascii());
			flush();
		}
		previous = text;
	}
	return logText;
}

QString Logger::logStartReceiver()
{
	return log(QString("START WAITING FOR CALLS"));
}

QString Logger::logStopReceiver()
{
	return log(QString("STOP WAITING FOR CALLS"));
}

QString Logger::logStop(QString address, long inBytes, long outBytes)
{
	return log(QString("HANG UP with %1: %2 bytes received, %3 bytes sent").arg(address).arg(inBytes).arg(outBytes));
}

QString Logger::logOutgoingCall(QString ip, QString address)
{
	return log(QString("CALL to %1 (%2)").arg(ip).arg(address));
}

QString Logger::logConnectedCall(QString ip, QString callerName)
{
	return log(QString("CONNECTED with %1 (%2)").arg(ip).arg(callerName));
}

QString Logger::logReceivedCall(QString ip, QString callerName)
{
	return log(QString("INCOMING CALL from %1 (%2)").arg(ip).arg(callerName));
}

QString Logger::logAbortedCall(QString ip, QString error)
{
	return log(QString("ABORTED CALL with %1 (%2)").arg(ip).arg(error));
}

void Logger::flush()
{
	fflush(logfile);
}

void Logger::end()
{
	if (logfile)
	{
		flush();
		fclose(logfile);
		logfile = NULL;
	}
}

QString Logger::getTime()
{
	return QDateTime::currentDateTime().toString();
}
