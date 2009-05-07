
TARGET	= server client

OBJ	= server.o client.o

CC	= gcc

LIBS = -lsctp

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
