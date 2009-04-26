
CC	= gcc

TARGET	= main

OBJ	= main.o

.PHONY: everything all clean distclean

everything: $(TARGET)

all: distclean everything

clean:
	rm -fv $(OBJ)

distclean: clean
	rm -fv $(TARGET)

main.o:main.c
	$(CC) -c $< -o $@

main:main.o
	$(CC) $< -o $@
