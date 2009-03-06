CPP  = g++
CC   = gcc
CFLAGS = -c -O2
LIBS =  -lpthread
RM = rm -rf

all :
	$(CC) $(CFLAGS) Common.c
	$(CC) $(CFLAGS) Server.c
	$(CC) $(CFLAGS) Client.c	
	$(CC) $(LIBS) Common.o Server.o -o Server
	$(CC) $(LIBS) Common.o Client.o -o Client
	$(RM) *.o


clean :
	$(RM) *.*~
	$(RM) *.o
	$(RM) *~
	$(RM) Server Client
