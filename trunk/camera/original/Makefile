
CC	= gcc

OBJ	= capture.o
TARGET	= capture

.PHONY: everything all clean clean_target

everything: $(TARGET)

all: clean_target everything

clean:
	rm -fv $(OBJ)
clean_target:clean
	rm -fv $(TARGET)

capture.o:capture.c
	$(CC) -c $< -o $@

capture:capture.o
	$(CC) $< -o $@
