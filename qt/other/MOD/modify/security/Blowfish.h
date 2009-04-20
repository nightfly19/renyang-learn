#ifndef BLOWFISH_H
#define BLOWFISH_H

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
