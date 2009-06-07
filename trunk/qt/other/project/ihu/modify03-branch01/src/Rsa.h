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

#ifndef ___RSA_HPP___
#define ___RSA_HPP___

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <unistd.h>
#include <gmpxx.h>

// This value should be enough for the moment
#define RSA_STRENGTH 1280

class Rsa
{
private:
	mpz_t d, e, my_n, n;
	mpz_t M, c;
	char *my_public_key;
	char *peer_public_key;
	int keylen;
	int pkeylen;
	int bitstrength;
	int primesize;
	int bufsize;
public:
	Rsa(int);
	~Rsa();
	
	void RSA_generateKeys();
	void initializeRandom();
	int encrypt(char *, int, char **);
	int decrypt(char*, int, char**);
	char* getMyPublicKey();
	int getMyPublicKeyLen();
	char* getPeerPublicKey();
	int getPeerPublicKeyLen();
	void resetPeerPublicKey();
	void setPeerPublicKey(char *, int);
	void convert2text(char *, int, char *);
	int convert2bin(char *, char *);
	static char hex2byte(char, char);
	static char hex2bin(char);
};

#endif
