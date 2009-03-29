CC	= gcc

TARGET	= server

OBJ	= server.o

LIBS	= -lpthread

.PHONY: everything all clean distclean

everything: $(TARGET)

all: distclean everything

clean:
	rm -fv $(OBJ)

distclean: clean
	rm -fv $(TARGET)

server.o:server.c
	$(CC) -c $< -o $@

server:server.o
	$(CC) $(LIBS) $< -o $@
