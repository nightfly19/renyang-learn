
ASM	= gcc

FLAGS	= -Wall -O2

SCTP_FLAGS = -lsctp

OBJ	= server.o client.o

TARGET 	= server client

.PHONY: everything all clean clean_target

everything: $(TARGET)

all: clean_target everything

clean:
	rm -fv $(OBJ)

clean_target:clean
	rm -fv $(TARGET)

server.o:server.c
	$(ASM) $(FLAGS) -c $< -o $@

server:server.o
	$(ASM) $(FLAGS) $(SCTP_FLAGS) $< -o $@

client.o:client.c
	$(ASM) $(FLAGS) -c $< -o $@

client:client.o
	$(ASM) $(FLAGS) $(SCTP_FLAGS) $< -o $@
