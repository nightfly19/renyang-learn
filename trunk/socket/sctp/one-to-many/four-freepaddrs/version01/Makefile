
CC	= gcc

TARGET	= server client

OBJ	= server.o client.o

LIBS	= -lsctp

.PHONY: everything all clean distclean

everything:$(TARGET)

all:distclean everything

clean:
	rm -fv $(OBJ)

distclean: clean
	rm -fv $(TARGET)

server.o:server.c
	$(CC) -c $< -o $@

server:server.o
	$(CC) $(LIBS) $< -o $@

client.o:client.c
	$(CC) -c $< -o $@

client:client.o
	$(CC) $(LIBS) $< -o $@
