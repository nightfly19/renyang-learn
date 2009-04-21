CC	= gcc

OBJ	= server.o

MAINPATH= ../../../../

LIBS	= $(MAINPATH)lib/libunp.a

CFLAGS	= -I$(MAINPATH)lib

TARGET	= server

.PHONY: everything all clean distclean

everything: $(TARGET)

all: distclean everything

clean:
	rm -fv $(OBJ)

distclean: clean
	rm -fv $(TARGET)
	$(MAKE) $@ -C $(MAINPATH)/lib

server.o: server.c
	$(CC) -c $< -o $@ $(CFLAGS)

server:server.o $(LIBS)
	$(CC) $< -o $@ $(LIBS)

$(LIBS):
	$(MAKE) -w -C $(MAINPATH)/lib
