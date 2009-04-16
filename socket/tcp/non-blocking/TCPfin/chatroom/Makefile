CPP  = g++
CC   = gcc
CFLAGS = -c -O2
LIBS =  -lpthread
RM = rm -rf

all :
	$(CC) $(CFLAGS) Common.c
	$(CC) $(CFLAGS) Ftransfer.c
	$(CC) $(CFLAGS) combine.c
	
	$(CC) $(LIBS) Ftransfer.o Common.o combine.o -o combine
	

	$(RM) *.o


clean :
	$(RM) *.*~
	$(RM) *.o
	$(RM) *~
	$(RM) combine Ftransfer
