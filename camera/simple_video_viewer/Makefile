# for the gtk
gtkflags=$(shell pkg-config --cflags gtk+-2.0)
gtklibs	=$(shell pkg-config --libs gtk+-2.0)

# for the video for linux
gtklibs += -lv4lconvert

CC	= gcc

TARGET	= svv
OBJ	= svv.o

.PHONY: everything all clean distclean

everything: $(TARGET)

all: distclean everything

clean:
	rm -fv $(OBJ)

distclean: clean
	rm -rfv $(TARGET)

svv.o:svv.c
	$(CC) $(gtkflags) -c $< -o $@

svv:svv.o
	$(CC) $(gtklibs) $< -o $@
