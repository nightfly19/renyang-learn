
CC	= gcc

OBJ	= main.o
TARGET	= main

.PHONY: everything all clean distclean

everything: $(TARGET)

all: distclean everything

clean:
	rm -fv $(OBJ) myfile.txt
distclean: clean
	rm -fv $(TARGET)

main.o: main.c
	$(CC) -c $< -o $@

main:main.o
	$(CC) $< -o $@
