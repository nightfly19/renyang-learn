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

/*
 *  The code below is a modified portion of speexenc.c
 *  Copyright (C) 2002-2003 Jean-Marc Valin, available
 *  with the official speex codec source codes at
 *  http://www.speex.org
*/

#include <string.h>
#include <errno.h>

#include "Error.h"
#include "Config.h"
#include "Ihu2Spx.h"

Ihu2Spx::Ihu2Spx(void)
{
	vendor_string = "Encoded with Speex";
	comments = NULL;
	comment_init(&comments, &comments_length, vendor_string);
	total_samples = 0;
	bytes_written = 0;
	fout = NULL;
}

Ihu2Spx::~Ihu2Spx(void)
{
	if (comments)
		free(comments);
	if (fout)
		fclose(fout);
}

#define readint(buf, base) (((buf[base+3]<<24)&0xff000000)| \
                           ((buf[base+2]<<16)&0xff0000)| \
                           ((buf[base+1]<<8)&0xff00)| \
  	           	    (buf[base]&0xff))
#define writeint(buf, base, val) do{ buf[base+3]=((val)>>24)&0xff; \
                                     buf[base+2]=((val)>>16)&0xff; \
                                     buf[base+1]=((val)>>8)&0xff; \
                                     buf[base]=(val)&0xff; \
                                 }while(0)

void Ihu2Spx::comment_init(char **comments, int* length, char *vendor_string)
{
	int vendor_length=strlen(vendor_string);
	int user_comment_list_length=0;
	int len=4+vendor_length+4;
	char *p=(char*)malloc(len);
	if(p!=NULL)
	{
		writeint(p, 0, vendor_length);
		memcpy(p+4, vendor_string, vendor_length);
		writeint(p, 4+vendor_length, user_comment_list_length);
		*length=len;
		*comments=p;
	}
}

#undef readint
#undef writeint

int Ihu2Spx::oe_write_page(ogg_page *page, FILE *fp)
{
	int written;
	written = fwrite(page->header,1,page->header_len, fp);
	written += fwrite(page->body,1,page->body_len, fp);
	return written;
}

void Ihu2Spx::setup(QString filename, const SpeexMode *mode, int rate, int fsize)
{
	if (fout)
		fclose(fout);
	filename.append(IHU_SPX_EXT);
	fout = fopen(filename.ascii(), "wb");
	if (fout)
	{
		total_samples = 0;
		bytes_written = 0;
		id = -1;
		frame_size = fsize;
		
		if (ogg_stream_init(&os, rand())==-1)
			throw Error(QString("stream init failed"));
	
		speex_init_header(&header, rate, 1, mode);
		header.frames_per_packet = 1;
		
		op.packet = (unsigned char *)speex_header_to_packet(&header, (int*)&(op.bytes));
		op.b_o_s = 1;
		op.e_o_s = 0;
		op.granulepos = 0;
		op.packetno = 0;
		ogg_stream_packetin(&os, &op);
		free(op.packet);
		
		op.packet = (unsigned char *)comments;
		op.bytes = comments_length;
		op.b_o_s = 0;
		op.e_o_s = 0;
		op.granulepos = 0;
		op.packetno = 1;
		ogg_stream_packetin(&os, &op);
		
		while((result = ogg_stream_flush(&os, &og)))
		{
			if(!result) break;
			ret = oe_write_page(&og, fout);
			if(ret != og.header_len + og.body_len)
				throw Error(QString("failed writing header to output stream"));
			else
				bytes_written += ret;
		}
	}
	else
	{
		throw Error(QString("%1: %2").arg(filename).arg(strerror(errno)));
}
}

void Ihu2Spx::process(char *cbits, int len)
{
	if (fout)
	{
		id++;
		
		total_samples += frame_size;
		
		op.packet = (unsigned char *)cbits;
		op.bytes = len;
		if (len)
			op.e_o_s = 0;
		else
			op.e_o_s = 1;
		op.b_o_s = 0;
		
		op.granulepos = (id+1)*frame_size;
		if (op.granulepos>total_samples)
			op.granulepos = total_samples;
		
		op.packetno = 2+id;
		ogg_stream_packetin(&os, &op);
		
		while (ogg_stream_pageout(&os,&og))
		{
			ret = oe_write_page(&og, fout);
			if(ret != og.header_len + og.body_len)
				throw Error(QString("failed writing header to output stream"));
			else
				bytes_written += ret;
		}
	}
}

void Ihu2Spx::end()
{
	if (total_samples)
	{
		process(NULL, 0); // Last empty packet with EOS = 1
		while (ogg_stream_flush(&os, &og))
		{
			ret = oe_write_page(&og, fout);
			if(ret != og.header_len + og.body_len)
				throw Error(QString("failed writing header to output stream"));
			else
				bytes_written += ret;
		}	
		ogg_stream_clear(&os);
		total_samples = 0;
	}
}

void Ihu2Spx::close()
{
	total_samples = 0;
	bytes_written = 0;
	if (fout)
		fclose(fout);
	fout = NULL;
}
