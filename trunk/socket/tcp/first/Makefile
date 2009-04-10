
ASM	= gcc
FLAGS	= 

OBJ=server.o client.o
TARGET=server client

.PHONY: everything clean all distclean

everything:$(TARGET)

distclean:clean
	rm -fv $(TARGET)

clean:
	rm -fv $(OBJ) copy

all: distclean everything

client.o: client.c
	$(ASM) $(FLAGS) -c $< -o $@
server.o: server.c
	$(ASM) $(FLAGS) -c $< -o $@
client:client.o
	$(ASM) $(FLAGS) $< -o $@
server:server.o
	$(ASM) $(FLAGS) $< -o $@
