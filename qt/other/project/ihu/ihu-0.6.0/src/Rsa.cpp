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
#include <time.h>

#include "Rsa.h"
#include "Error.h"

Rsa::Rsa(int strength)
{
	mpz_init(d);
	mpz_init(e);
	mpz_init(my_n);
	mpz_init(n);
	
	mpz_init(M);
	mpz_init(c);
	
	srand(time(NULL));
	
	bitstrength = strength;
	primesize = bitstrength/2;
	bufsize = bitstrength;
	
	my_public_key = new char[bufsize];
	peer_public_key = NULL;
	pkeylen = 0;
	
	RSA_generateKeys();
}

Rsa::~Rsa()
{
	mpz_clear(d);
	mpz_clear(e);
	mpz_clear(my_n);
	mpz_clear(n);
	
	mpz_clear(M);
	mpz_clear(c);
	
	if (peer_public_key)
		free(peer_public_key);
	
	delete[] my_public_key;
}

void Rsa::RSA_generateKeys()
{
	mpz_t p,q;
	mpz_init(p);
	mpz_init(q);
	char* p_str = new char[primesize+1];
	char* q_str = new char[primesize+1];
	
	p_str[0] = '1';
	q_str[0] = '1';
	
	for(int i=1;i<primesize;i++)
		p_str[i] = (int)(2.0*rand()/(RAND_MAX+1.0)) + 0x30;
	for(int i=1;i<primesize;i++)
		q_str[i] = (int)(2.0*rand()/(RAND_MAX+1.0)) + 0x30;
	
	p_str[primesize] = '\0';
	q_str[primesize] = '\0';
	
	mpz_set_str(p,p_str,2);
	mpz_set_str(q,q_str,2);
	
	mpz_nextprime(p,p);
	mpz_nextprime(q,q);
	
	mpz_t x;
	mpz_init(x);
	mpz_mul(my_n,p,q);
	
	char *n_str = new char[bufsize];
	mpz_get_str(n_str, 16, my_n);
	keylen = convert2bin(n_str, my_public_key);
	
	mpz_t p_minus_1,q_minus_1;
	mpz_init(p_minus_1);
	mpz_init(q_minus_1);
	mpz_sub_ui(p_minus_1, p, (unsigned long int)1);
	mpz_sub_ui(q_minus_1, q, (unsigned long int)1);
	mpz_mul(x, p_minus_1, q_minus_1);
	
	mpz_t gcd;
	mpz_init(gcd);
	
	unsigned long int e_int = 65537;
	
	while(true)
	{
		mpz_gcd_ui(gcd, x, e_int);
		if(mpz_cmp_ui(gcd, (unsigned long int)1)==0)
		break;
		e_int += 2;
	}
	mpz_set_ui(e, e_int);
	
	if(mpz_invert(d, e, x)==0)
	{
		RSA_generateKeys();
	}
	
	mpz_clear(p);
	mpz_clear(q);
	mpz_clear(x);
	mpz_clear(p_minus_1);
	mpz_clear(q_minus_1);
	mpz_clear(gcd);
	delete[] p_str;
	delete[] q_str;
	delete[] n_str;
}

int Rsa::encrypt(char *data, int len, char **output)
{
	char ciphertext[bufsize], text[bufsize];
	
	memset(text, 0x0, bufsize);
	convert2text(peer_public_key, pkeylen, text);
	
	mpz_set_str(n, text, 16);
	
	memset(ciphertext, 0x0, bufsize);
	memset(text, 0x0, bufsize);
	
	convert2text(data, len, text);
	
	mpz_set_str(M, text, 16);
	mpz_powm(c, M, e, n);
	mpz_get_str(text, 16, c);
	
	if ((*output = (char *) malloc(strlen(text)+1)) == NULL)
		return 0;
	
	return convert2bin(text, *output);
}

int Rsa::decrypt(char *ciphertext, int len, char **decrypted)
{
	char temp[bufsize];
	
	memset(temp, 0x0, bufsize);
	convert2text(ciphertext, len, temp);
	
	mpz_set_str(c, temp, 16);
	mpz_powm(M, c, d, my_n);
	mpz_get_str(temp, 16, M);
	if ((*decrypted = (char *) malloc(bufsize)) == NULL)
		return 0;
	memset(*decrypted, 0x0, bufsize);
	
	return convert2bin((char *) temp, (char *) *decrypted);
}

char* Rsa::getMyPublicKey()
{
	return my_public_key;
}

int Rsa::getMyPublicKeyLen()
{
	return keylen;
}

char* Rsa::getPeerPublicKey()
{
	return peer_public_key;
}

int Rsa::getPeerPublicKeyLen()
{
	return pkeylen;
}

void Rsa::resetPeerPublicKey()
{
	if (peer_public_key)
		free(peer_public_key);
	pkeylen = 0;
}

void Rsa::setPeerPublicKey(char *key, int len)
{
	resetPeerPublicKey();
	if ((peer_public_key = (char *) malloc (len)) == NULL)
		throw Error(Error::IHU_ERR_MEMORY);
	memcpy(peer_public_key, key, len);
	pkeylen = len;
}

void Rsa::convert2text(char *input, int len, char *output)
{
	char tmps[3];
	for(int i=0; i<len; i++)
	{
		sprintf(tmps,"%02x", (unsigned char) input[i]);
		strcat(output, tmps);
	}
}

int Rsa::convert2bin(char *input, char *output)
{
	char temp[bufsize];
	int i, j = 0, len = 0;
	
	if(strlen(input)%2 == 1)
	{
		strcpy(temp,"0");
		strcat(temp,input);
	}
	else
		strcpy(temp,input);
	
	len = strlen(temp);
	for (i=0; i<=(len-2); i+=2)
	{
		output[j++] = (char) hex2byte(temp[i], temp[i+1]);
	}
	return j;
}

char Rsa::hex2byte(char c1, char c2)
{
	char ret = hex2bin(c1);
	ret = (ret << 4) | hex2bin(c2);
	return ret;
}

char Rsa::hex2bin(char c)
{
	if ((c >= 0x30) && (c <= 0x39))
		return (c - 0x30);
	if ((c >= 0x61) && (c <= 0x66))
		return (c - 0x57);
	return 0x0;
}
