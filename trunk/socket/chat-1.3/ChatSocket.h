//ChatSocket.h
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

#ifndef _CHATSOCKET_H
#define _CHATSOCKET_H

#include <TcpSocket.h>


enum {
	STATE_LOGIN = 1,
	STATE_PROMPT,
	STATE_QUIT
};


class ChatSocket : public TcpSocket
{
public:
	ChatSocket(ISocketHandler& );
	~ChatSocket();

	void OnAccept();
	void OnLine(const std::string& );

	const std::string& GetName() { return m_name; }

private:
	void SendPrompt();
	int m_state;
	std::string m_name;
};


#endif // _CHATSOCKET_H
