
CC	= gcc

TARGET	= main

OBJ	= main.o

CFLAGS	= `pkg-config opencv --cflags`

LIBS	= `pkg-config opencv --libs`

.PHONY: everything all clean distclean

everything: $(TARGET)

all: distclean everything

clean:
	rm -fv $(OBJ)

distclean: clean
	rm -fv $(TARGET)

main.o:main.c
	$(CC) $(CFLAGS) -c $< -o $@

main:main.o
	$(CC) $(LIBS) $< -o $@
