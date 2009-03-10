
CC	= gcc

TARGET	= main

OBJ	= main.o

.PHONY: everything all clean clean_target

everything:$(TARGET)

all: clean_target everything

clean:
	rm -fv $(OBJ)

clean_target: clean
	rm -fv $(TARGET)

main.o:main.c
	$(CC) -c $< -o $@

main:main.o
	$(CC) $< -o $@
