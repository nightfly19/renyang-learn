
CC	= gcc

C_LANG_OPT = -fopenmp

TARGET	= main

OBJ	= main.o

.PHONY: everything all clean clean_target

everything: $(TARGET)

all: clean_target everything

clean:
	rm -fv $(OBJ)

clean_target: clean
	rm -fv $(TARGET)

main.o:main.c
	$(CC) $(C_LANG_OPT) -c $< -o $@

main:main.o
	$(CC) $(C_LANG_OPT) $< -o $@
