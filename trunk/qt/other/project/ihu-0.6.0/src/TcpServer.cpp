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

#include <qstring.h>
#include <qsocketdevice.h>
#include "TcpServer.hpp"
#include "Error.h"

TcpServer::TcpServer( QObject* parent=0, int port = 0) :
        QServerSocket(port,1,parent)
{
        if ( !ok() )
		throw Error(tr("Can't listen on TCP port %1 (already in use?)").arg(port));
}

void TcpServer::newConnection( int socket )
{
	emit newConnect(socket);
}
