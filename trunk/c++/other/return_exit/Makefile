
ASM	= g++

OBJ	= main.o

TARGET	= main

.PHONY:everything all clean distclean

everything:$(TARGET)

clean:
	rm -fv $(OBJ)

distclean: clean
	rm -fv $(TARGET)

all: distclean everything

main.o:main.cpp
	$(ASM) -c $< -o $@

main:main.o
	$(ASM) $< -o $@
