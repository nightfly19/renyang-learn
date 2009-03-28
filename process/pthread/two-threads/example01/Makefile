
CC	= gcc

LIBS	+= -lpthread

OBJ	= pthread1.o

TARGET	= pthread1

.PHONY:everything all clean distclean

everything: $(TARGET)

all: distclean all

clean:
	rm -fv $(OBJ)

distclean: clean
	rm -fv $(TARGET)

pthread1.o:pthread1.c
	$(CC) -c $< -o $@

pthread1:pthread1.o
	$(CC) $(LIBS) $< -o $@
