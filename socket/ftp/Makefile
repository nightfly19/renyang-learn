
ASM	= gcc

OBJ=server.o client.o
TARGET=server client

.PHONY: everything clean all clean_target

everything:$(TARGET)

clean_target:clean
	rm -fv $(TARGET)

clean:
	rm -fv $(OBJ)

all: clean_target everything

client.o: client.c
	$(ASM) -c $< -o $@
server.o: server.c
	$(ASM) -c $< -o $@
client:client.o
	$(ASM) $< -o $@
server:server.o
	$(ASM) $< -o $@
