CC	= gcc

TARGET	= server

OBJ	= server.o

LIBS	= -lsctp

.PHONY: everything all clean distclean

everything: $(TARGET)

all: distclean everything

clean:
	rm -fv $(OBJ)

distclean: clean
	rm -fv $(TARGET)

%.o:%.c
	$(CC) -c $< -o $@

server:server.o
	$(CC) $(LIBS) $< -o $@
