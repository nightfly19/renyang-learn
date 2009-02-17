
gtkflags=$(shell pkg-config --cflags gtk+-2.0)
gtklibs	=$(shell pkg-config --libs gtk+-2.0)

CC	= gcc

TARGET	= svv
OBJ	= svv.o

.PHONY: everything all clean clean_target

everything: $(TARGET)

all: clean_target everything

clean:
	rm -fv $(OBJ)

clean_target: clean
	rm -rfv $(TARGET)

svv.o:svv.c
	$(CC) $(gtkflags) -c $< -o $@

svv:svv.o
	$(CC) $(gtklibs) $< -o $@
