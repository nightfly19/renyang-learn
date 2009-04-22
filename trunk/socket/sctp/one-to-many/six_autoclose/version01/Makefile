
CC	= gcc

TARGET	= server client

OBJ	= server.o client.o

LIBS	= -lsctp

ifeq ($(UN_MOD),on)
	MOD_FLAG = -DUN_MOD
endif

.PHONY: everything all clean distclean

everything:$(TARGET)

all:distclean everything

clean:
	rm -fv $(OBJ)

distclean: clean
	rm -fv $(TARGET)

server.o:server.c
	$(CC) $(MOD_FLAG) -c $< -o $@

server:server.o
	$(CC) $(LIBS) $< -o $@

client.o:client.c
	$(CC) $(MOD_FLAG) -c $< -o $@

client:client.o
	$(CC) $(LIBS) $< -o $@
