
CC	= gcc

OBJ	= tcpserv01.o tcpcli01.o

TARGET	= tcpserv01 tcpcli01

LIBS	= -lpthread

.PHONY: everything all clean distclean

everything: $(TARGET)

all: distclean everything

clean:
	rm -fv $(OBJ)

distclean: clean
	rm -fv $(TARGET)

tcpserv01.o:tcpserv01.c
	$(CC) -c $< -o $@

tcpcli01.o:tcpcli01.c
	$(CC) -c $< -o $@

tcpserv01:tcpserv01.o
	$(CC) $(LIBS) $< -o $@

tcpcli01:tcpcli01.o
	$(CC) $(LIBS) $< -o $@
