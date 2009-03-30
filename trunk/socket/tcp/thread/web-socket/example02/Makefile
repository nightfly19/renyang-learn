CC	= gcc

TARGET	= client

OBJ	= client.o

LIBS	= -lpthread

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
	$(CC) $(LIBS) $< -o $@
