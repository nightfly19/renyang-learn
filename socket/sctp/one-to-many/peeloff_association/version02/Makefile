
CC	= gcc

LIBS	= -lsctp

OBJ	= server.o client.o

TARGET	= server client

.PHONY: everything all clean distclean

everything: $(TARGET)

all: distclean everything

clean:
	rm -fv $(OBJ)

distclean: clean
	rm -fv $(TARGET)

server:server.o
	$(CC) $(LIBS) $< -o $@

client:client.o
	$(CC) $(LIBS) $< -o $@
