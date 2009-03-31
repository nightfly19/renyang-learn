CC	= gcc

OBJ	= client.o

TARGET	= client

.PHONY: everything all clean distclean

everything: $(TARGET)

all: distclean everything

clean:
	rm -fv $(OBJ)

distclean: clean
	rm -fv $(TARGET)

client.o:client.c
	$(CC) -c $< -o $@

client:client.o
	$(CC) $< -o $@
