
CC	= gcc

FLAGS	= -DMARS

TARGET	= hello mars

OBJ	= hello.o mars.o

.PHONY: everything all clean distclean

everything: $(TARGET)

all: distclean everything

clean:
	rm -fv $(OBJ)

distclean: clean
	rm -fv $(TARGET)

hello.o:hello.c
	$(CC) -c $< -o $@
mars.o:hello.c
	$(CC) $(FLAGS) -c $< -o $@
hello:hello.o
	$(CC) $< -o $@
mars:mars.o
	$(CC) $< -o $@
