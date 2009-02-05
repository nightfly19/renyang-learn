
ASM	= g++

OBJ	= main.o

TARGET	= main

.PHONY:everything all clean clean_target

everything:$(TARGET)

clean:
	rm -fv $(OBJ)

clean_target: clean
	rm -fv $(TARGET)

all: clean_target everything

main.o:main.cpp
	$(ASM) -c $< -o $@

main:main.o
	$(ASM) $< -o $@
