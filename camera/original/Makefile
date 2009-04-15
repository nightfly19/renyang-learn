
CC	= gcc

OBJ	= capture.o
TARGET	= capture

.PHONY: everything all clean distclean

everything: $(TARGET)

all: distclean everything

clean:
	rm -fv $(OBJ)
distclean:clean
	rm -fv $(TARGET)

capture.o:capture.c
	$(CC) -c $< -o $@

capture:capture.o
	$(CC) $< -o $@
