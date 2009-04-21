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

#ifndef IHU2SPX_HPP
#define IHU2SPX_HPP

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <qstring.h>

#include <speex/speex.h>
#include <speex/speex_header.h>
#include <ogg/ogg.h>

#define MAX_FRAME_SIZE 2000
#define MAX_FRAME_BYTES 2000

class Ihu2Spx
{
private:
	FILE *fout;
	ogg_stream_state os;
	ogg_page og;
	ogg_packet op;
	SpeexHeader header;
	char *vendor_string;
	char *comments;
	int comments_length;
	int bytes_written, ret, result;
	int total_samples, frame_size, id;

public:
	Ihu2Spx();
	~Ihu2Spx();
	void setup(QString, const SpeexMode *, int, int);
	void end();
	void close();
	void comment_init(char **, int*, char *);
	int oe_write_page(ogg_page *, FILE *);
	void process(char *, int);
};

#endif
