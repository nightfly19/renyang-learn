
CC	= gcc

CFLAGS	= `pkg-config opencv --cflags`

LIBS	= `pkg-config opencv --libs`

OBJ	= fdiff.o

TARGET	= fdiff

.PHONY: everything all clean distclean

everything:$(TARGET)

all: distclean everything

clean:
	rm -fv $(OBJ)

distclean: clean
	rm -fv $(TARGET)

fdiff.o:fdiff.c
	$(CC) $(CFLAGS) -c $< -o $@

fdiff:fdiff.o
	$(CC) $(LIBS) $< -o $@
