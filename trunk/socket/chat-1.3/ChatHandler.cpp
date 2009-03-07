//ChatHandler.cpp
/*
Copyright (C) 2004  Anders Hedstrom

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

//#include <stdio.h>

#include "ChatSocket.h"
#include "ChatHandler.h"




ChatHandler::ChatHandler()
:SocketHandler()
{
}


ChatHandler::~ChatHandler()
{
}


void ChatHandler::Talk(const std::string& name,const std::string& line)
{
	std::string str = "\n";
	str += name + " says '" + line + "'\n";

	for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
	{
//		SOCKET s = (*it).first;
		Socket *p0 = (*it).second;
		ChatSocket *p = dynamic_cast<ChatSocket *>(p0);
		if (p)
		{
			p -> Send(str);
		}
	}
}


void ChatHandler::Who(ChatSocket *p2)
{
	for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
	{
//		SOCKET s = (*it).first;
		Socket *p0 = (*it).second;
		ChatSocket *p = dynamic_cast<ChatSocket *>(p0);
		if (p)
		{
			p2 -> Send(p -> GetName() + "\n");
		}
	}
}


