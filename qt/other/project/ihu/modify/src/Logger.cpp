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

// renyang - 在開始一個新的檔案之前，要先結束之前所控制的檔案
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
		// renyang - 若檔案不是空白, 才寫入資料
		if (logfile)
		{
			fprintf(logfile, "%s\n", logText.ascii());
			flush();
		}
		// renyang - 設定previous用來避免資料重覆記錄
		previous = text;
	}
	// renayng - 回傳記錄到log檔的資料, 包含時間
	return logText;
}

// renyang - 記錄開始接收
QString Logger::logStartReceiver()
{
	return log(QString("START WAITING FOR CALLS"));
}

// renyang - 記錄停止接收
QString Logger::logStopReceiver()
{
	return log(QString("STOP WAITING FOR CALLS"));
}

// renyang - 記錄掛斷電話, 並且同時記錄接收幾個bytes與收到幾個byte
QString Logger::logStop(QString address, long inBytes, long outBytes)
{
	return log(QString("HANG UP with %1: %2 bytes received, %3 bytes sent").arg(address).arg(inBytes).arg(outBytes));
}

// renyang - 記錄打出去的電話ip@@@
QString Logger::logOutgoingCall(QString ip, QString address)
{
	return log(QString("CALL to %1 (%2)").arg(ip).arg(address));
}

// renyang - 記錄連到的ip與對方的名稱
QString Logger::logConnectedCall(QString ip, QString callerName)
{
	return log(QString("CONNECTED with %1 (%2)").arg(ip).arg(callerName));
}

// renyang - 記錄接收到的電話時，是由哪一個ip與使用者打過來的
QString Logger::logReceivedCall(QString ip, QString callerName)
{
	return log(QString("INCOMING CALL from %1 (%2)").arg(ip).arg(callerName));
}

// renyang - 通話中出現錯誤的ip與錯誤訊息
QString Logger::logAbortedCall(QString ip, QString error)
{
	return log(QString("ABORTED CALL with %1 (%2)").arg(ip).arg(error));
}

// renyang - 把目前記憶體的資料存放到所設定的logfile檔案中
void Logger::flush()
{
	fflush(logfile);
}

// renyang - 把還在記憶體的資料存放到檔案中, 並且結束控制
void Logger::end()
{
	if (logfile)
	{
		flush();
		// renyang - 結束檔案
		fclose(logfile);
		// renyang - 對logfile結束控制
		logfile = NULL;
	}
}

// renyang - 取得目前的時間
QString Logger::getTime()
{
	return QDateTime::currentDateTime().toString();
}
