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

#ifndef ___BLOWFISH_HPP___
#define ___BLOWFISH_HPP___

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define NUM_SUBKEYS  18
#define NUM_S_BOXES  4
#define NUM_ENTRIES  256

#define MAX_PASSWD   56  // 448bits

#if BYTE_ORDER == LITTLE_ENDIAN
struct WordByte
{
	unsigned int three:8;
	unsigned int two:8;
	unsigned int one:8;
	unsigned int zero:8;
};
#else
struct WordByte
{
	unsigned int zero:8;
	unsigned int one:8;
	unsigned int two:8;
	unsigned int three:8;
};
#endif

union Word
{
	unsigned int word;
	WordByte byte;
};

struct DWord
{
	Word word0;
	Word word1;
};


class Blowfish
{
private:
	unsigned int PA[NUM_SUBKEYS];
	unsigned int SB[NUM_S_BOXES][NUM_ENTRIES];
	
	void genSubKeys();
	inline void BF_En(Word *,Word *);
	inline void BF_De(Word *,Word *);
	
	char *passwd;
	int passlen;
public:
	Blowfish(char *, int len);
	~Blowfish();
	
	void reset();
	void setPasswd(char *, int);
	void encrypt(void *,unsigned int);
	void decrypt(void *,unsigned int);
	char *getPass();
	int getPassLen();
	bool isAsciiKey();
};

#endif
