CC	= gcc

TARGET	= client server

OBJ	= client.o server.o

MAINPATH= ../../../..

CFLAGS	= -I$(MAINPATH)/lib

LIBS	= -lsctp

CUSTOMER	= $(MAINPATH)/lib/libunp.a

.PHONY: everything all clean distclean

everything: $(TARGET)

all: distclean everything

clean:
	rm -fv $(OBJ)

distclean: clean
	rm -fv $(TARGET)

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

server:server.o
	$(CC) $(LIBS) $< $(CUSTOMER) -o $@

client:client.o
	$(CC) $(LIBS) $< $(CUSTOMER) -o $@
