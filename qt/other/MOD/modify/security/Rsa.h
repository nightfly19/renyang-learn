#ifndef RSA_H
#define RSA_H

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
