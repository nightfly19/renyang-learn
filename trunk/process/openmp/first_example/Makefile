
CC	= gcc

C_LANG_OPT = -fopenmp

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
	$(CC) $(C_LANG_OPT) -c $< -o $@

main:main.o
	$(CC) $(C_LANG_OPT) $< -o $@
