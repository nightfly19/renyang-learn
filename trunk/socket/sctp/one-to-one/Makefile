
ASM	= gcc

OBJ	= server.o

TARGET	= server

.PHONY: everything clean clean_target all

everything: $(TARGET)

clean:
	rm -rf $(OBJ)

clean_target:clean
	rm -rf $(TARGET)

all: clean_target everything

server.o:server.c
	$(ASM) -c $< -o $@

server: server.o
	$(ASM) $< -o $@
