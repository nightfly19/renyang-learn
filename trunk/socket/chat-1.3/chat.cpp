//chat.cpp
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

#include <stdio.h>
#include <signal.h>
#include <ListenSocket.h>
#include "ChatHandler.h"
#include "ChatSocket.h"

#define DEB(x) 


static	int m_quit = 0;


#ifndef _WIN32
void sigint(int s) /* save+quit */
{
DEB(	printf("sigint\n");)
	m_quit++;
}

void sighup(int s) /* quit */
{
DEB(	printf("sighup\n");)
	m_quit++;
}

void sigusr1(int s) /* save */
{
}

void sigusr2(int s) /* reset all */
{
}

void sigpipe(int s)
{
}

void siginit(void)
{
	signal(SIGINT, (__sighandler_t)sigint);
	signal(SIGHUP, (__sighandler_t)sighup);
	signal(SIGUSR1, (__sighandler_t)sigusr1);
	signal(SIGUSR2, (__sighandler_t)sigusr2);
	signal(SIGPIPE, (__sighandler_t)sigpipe);
}
#endif


int main(int argc,char *argv[])
{
	ChatHandler h;
	ListenSocket<ChatSocket> l(h);
	siginit();

	if (l.Bind(4440))
	{
		exit(-1);
	}
	h.Add(&l);
	while (!m_quit)
	{
		h.Select(1,0);
	}
}

