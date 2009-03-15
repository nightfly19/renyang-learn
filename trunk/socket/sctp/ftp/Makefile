
ASM	= gcc
FLAG	= -lsctp

OBJ	= server.o client.o
TARGET	= server client

.PHONY: everything all clean clean_target

everything: $(TARGET)

all: clean_target everything

clean:
	rm -fv $(OBJ)

clean_target: clean
	rm -fv $(TARGET)

server.o:server.c
	$(ASM) -c $< -o $@

server:server.o
	$(ASM) $(FLAG) $< -o $@

client.o:client.c
	$(ASM) -c $< -o $@

client:client.o
	$(ASM) $(FLAG) $< -o $@
