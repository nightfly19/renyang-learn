
CC	= gcc

LIBS	+= -lpthread

OBJ	= pthread1.o

TARGET	= pthread1

.PHONY:everything all clean clean_target

everything: $(TARGET)

all: clean_target all

clean:
	rm -fv $(OBJ)

clean_target: clean
	rm -fv $(TARGET)

pthread1.o:pthread1.c
	$(CC) -c $< -o $@

pthread1:pthread1.o
	$(CC) $(LIBS) $< -o $@
