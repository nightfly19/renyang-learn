//========================include file========================
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <pthread.h>
#include <sys/sem.h>
//============================================================

//========================define function=====================
void reader_function(void);
void writer_function(void);
//============================================================

//========================global variable=====================
char buffer;
//============================================================

//========================main function=======================
int main()
{
	pthread_t reader;

	return 0;
}

