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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <qapplication.h>

#include "Config.h"
#include "Error.h"
#include "Ihu.hpp"
#include "IhuNoGui.hpp"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

static Ihu *ihu = NULL;
static IhuNoGui *ihuNoGui = NULL;

void terminate(int val)
{
	fprintf(stderr, "SIGINT caught. Terminating...\n");
	if (ihu)
	{
		ihu->quit();
	}
	else
	if (ihuNoGui)
	{
		ihuNoGui->quit();
		delete ihuNoGui;
		exit(0);
	}
}

void usage(int ret)
{
	fprintf(stdout, "Welcome to IHU v. %s by Matteo Trotta <%s>\nUSAGE\n\tihu [OPTIONS]\n", PACKAGE_VERSION, PACKAGE_BUGREPORT);
	fprintf(stdout, "OPTIONS\n\t--conf\tFILE\tUse FILE as configuration file\n\t--wait\t\tWait for incoming calls\n");
	fprintf(stdout, "\t--call\tHOST\tCall HOST (DNS or IP address)\n\t--file\tFILE\tPlay a IHU FILE\n");
	fprintf(stdout, "\t--encrypt\tEncrypt the outgoing stream\n\t--noinput\tDisable audio INPUT\n\t--nooutput\tDisable audio OUTPUT\n");
	fprintf(stdout, "\t--nogui\t\tStart without GUI (no interaction)\n\n");
	if (ret)
		fprintf(stderr, "Error: one or more argument is missing.\n");
	exit(ret);
}

int main( int argc, char **argv )
{
	bool enableGui = true;
	int ret = 0;

	signal(SIGINT, terminate);
	
	/*
		Those options are useful for quick launch (at least for me ;),
		but I don't think it's a good idea to use them (they might be buggy),
		in the future I may use standard GNU getopt, but for the moment
		please use the graphic interface instead.
	*/
	int waitcall = 0, call = 0, file = 0, muteMic = 0, muteSpk = 0, encrypt = 0, conf = 0;
	char* cfg = NULL;

	for(int i=1; i<argc; i++)
	{
		if (strcmp(argv[i], "--help")==0)
			usage(0);
		else
		if (strcmp(argv[i], "--nogui")==0)
			enableGui = false;
		else
		if (strcmp(argv[i], "--conf")==0)
			conf = i+1;
		else
		if (strcmp(argv[i], "--wait")==0)
			waitcall = 1;
		else
		if (strcmp(argv[i], "--call")==0)
			call = i+1;
		else
		if (strcmp(argv[i], "--file")==0)
			file = i+1;
		else
		if (strcmp(argv[i], "--noinput")==0)
			muteMic = 1;
		else
		if (strcmp(argv[i], "--nooutput")==0)
			muteSpk = 1;
		else
		if (strcmp(argv[i], "--encrypt")==0)
			encrypt = 1;
	}
	
 	if (conf)
	{
		if (argc < (conf + 1))
			usage(1);
	}
	if (file)
	{
		if (argc < (file + 1))
			usage(1);
	}
 	if (call)
	{
		if (argc < (call + 1))
			usage(1);
	}

	try
	{
		if (conf)
		{
			cfg = argv[conf];
			fprintf(stdout, "Using '%s' as configuration file\n", argv[conf]);
		}

		Config ihuconfig(cfg);
		
		QApplication a( argc, argv, enableGui );
		
		if (enableGui)
		{
			ihu = new Ihu(ihuconfig);
			a.setMainWidget( ihu );
			
			ihu->initIhu();
			
			if (muteMic)
				ihu->disableIn();
			if (muteSpk)
				ihu->disableOut();
			if (encrypt)
				ihu->cryptOn();
			
			if (waitcall)
				ihu->waitForCalls();
	
			if (file)
				ihu->playFile(argv[file]);
			else
			if (call)
				ihu->call(argv[call]);
	
			ret = a.exec();
	
			delete ihu;
		}
		else
		{
			fprintf(stdout, "Starting IHU without GUI...\nPress CTRL-C to terminate\n");

			if (!waitcall && !call && !file)
				usage(1);
			
			ihuNoGui = new IhuNoGui(ihuconfig);
			
			ihuNoGui->initIhu();
			
			if (muteMic)
				ihuNoGui->disableIn();
			if (muteSpk)
				ihuNoGui->disableOut();
			if (encrypt)
				ihuNoGui->cryptOn();
			
			if (waitcall)
				ihuNoGui->waitForCalls();
			else
			if (call)
				ihuNoGui->call(argv[call]);
			else
			if (file)
				ihuNoGui->playFile(argv[file]);
			
			ret = a.exec();
	
			delete ihuNoGui;
		}

		ihuconfig.writeConfig();
	}
	catch (Error e)
	{
		fprintf(stderr, "ABORTED.\nError: %s\n", e.getError());
		ret = 1;
	}
	
	return ret;
}
