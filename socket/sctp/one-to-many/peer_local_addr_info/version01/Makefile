CC	= gcc

TARGET	= client server

OBJ	= client.o server.o

MAINPATH= ../../../..

CFLAGS	= -I$(MAINPATH)/lib

LIBS	= -lsctp

LIBUNP	= $(MAINPATH)/lib/libunp.a

.PHONY: everything all clean distclean

everything: $(TARGET)

all: distclean everything

clean:
	rm -fv $(OBJ)

distclean: clean
	rm -fv $(TARGET)
	$(MAKE) $@ -C $(MAINPATH)/lib

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

server:server.o $(LIBUNP)
	$(CC) $(LIBS) $< $(LIBUNP) -o $@

client:client.o $(LIBUNP)
	$(CC) $(LIBS) $< $(LIBUNP) -o $@

$(LIBUNP):
	$(MAKE) -C $(MAINPATH)/lib
