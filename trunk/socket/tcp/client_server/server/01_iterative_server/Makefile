CC	= gcc

OBJ	= server.o

LIBS	= ../../lib/libunp.a

CFLAGS	= -I../../lib

TARGET	= server

.PHONY: everything all clean distclean

everything: $(TARGET)

all: distclean everything

clean:
	rm -fv $(OBJ)

distclean: clean
	rm -fv $(TARGET)

server.o: server.c
	$(CC) -c $< -o $@ $(CFLAGS)

server:server.o $(LIBS)
	$(CC) $< -o $@ $(LIBS)

$(LIBS):
	$(MAKE) -w -C ../../lib
