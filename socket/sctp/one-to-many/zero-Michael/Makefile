CC	= gcc

LIBS	= -lsctp

TARGET	= server

OBJ	= server.o

.PHONY: everything all clean distclean

everything: $(TARGET)

all: distclean everything

clean:
	rm -fv $(OBJ)

distclean: clean
	rm -fv $(TARGET)

server.o:Michael_server.c
	$(CC) -c $< -o $@

server:server.o
	$(CC) $(LIBS) $< -o $@
