

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
	    throw Error(QString("can't open file (%1)").arg(strerror(errno)));
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

QString Logger::logStopReceiver(int calls, int connections)
{
    return log(QString("RECEIVER STOP. Missed %1 calls (%2 connections).\n").arg(calls).arg(connections));
}

QString Logger::logStop(long inBytes, long outBytes)
{
    return log(QString("HANG UP. %1 bytes received, %2 bytes sent.\n").arg(inBytes).arg(outBytes));
}

QString Logger::logOutgoingCall(QString address, QString ip)
{
    return log(QString("Outgoing call to %1 (%2)").arg(address).arg(ip));
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
