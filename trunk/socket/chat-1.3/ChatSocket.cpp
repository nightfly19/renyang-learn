//ChatSocket.cpp
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

#include "ChatHandler.h"
#include "ChatSocket.h"




ChatSocket::ChatSocket(ISocketHandler& h)
:TcpSocket(h)
,m_state(STATE_LOGIN)
,m_name("")
{
	SetLineProtocol();
}


ChatSocket::~ChatSocket()
{
}


void ChatSocket::OnAccept()
{
	Send("Welcome.\n");
	SendPrompt();
}


void ChatSocket::OnLine(const std::string& line)
{
	switch (m_state)
	{
	case STATE_LOGIN:
		if (line.size())
		{
			m_name = line;
			m_state = STATE_PROMPT;
		}
		SendPrompt();
		break;
	case STATE_PROMPT:
		if (line.size())
		{
			if (line == "/who")
			{
				static_cast<ChatHandler&>(Handler()).Who(this);
			}
			else
			if (line == "/quit")
			{
				m_state = STATE_QUIT;
			}
			else
			{
				static_cast<ChatHandler&>(Handler()).Talk(m_name,line);
			}
		}
		SendPrompt();
		break;
	case STATE_QUIT:
		break;
	}
}


void ChatSocket::SendPrompt()
{
	switch (m_state)
	{
	case STATE_LOGIN:
		Send("Enter name: ");
		break;
	case STATE_PROMPT:
		Send("> ");
		break;
	case STATE_QUIT:
		Send("Goodbye!\n");
		SetCloseAndDelete();
		break;
	}
}


