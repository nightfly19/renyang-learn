
ASM	= gcc

OBJ	= client.o server.o

TARGET	= client server

.PHONY: everything all clean distclean

everything:$(TARGET)

all: distclean everything

clean:
	rm -fv $(OBJ)

distclean:clean
	rm -fv $(TARGET)

client.o:client.c
	$(ASM) -c $< -o $@

server.o:server.c
	$(ASM) -c $< -o $@

client:client.o
	$(ASM) $< -o $@

server:server.o
	$(ASM) $< -o $@
