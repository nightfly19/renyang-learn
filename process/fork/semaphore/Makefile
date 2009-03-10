
CC	= gcc

OBJ	= main.o

TARGET	= main

LIBS	= -lpthread

.PHONY: everything all clean clean_target

everything: $(TARGET)

all: clean_target everything

clean:
	rm -fv $(OBJ)

clean_target: clean
	rm -fv $(TARGET)

main.o:main.c
	$(CC) -c $< -o $@

main:main.o
	$(CC) $(LIBS) $< -o $@
