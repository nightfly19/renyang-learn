
ASM	= gcc
FLAG	= -lsctp

OBJ	= server.o client.o
TARGET	= server client

.PHONY: everything all clean distclean

everything: $(TARGET)

all: distclean everything

clean:
	rm -fv $(OBJ)

distclean: clean
	rm -fv $(TARGET)

server.o:server.c
	$(ASM) -c $< -o $@

server:server.o
	$(ASM) $(FLAG) $< -o $@

client.o:client.c
	$(ASM) -c $< -o $@

client:client.o
	$(ASM) $(FLAG) $< -o $@
