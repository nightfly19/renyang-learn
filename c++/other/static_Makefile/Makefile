
ASM	= g++

OBJ=food.o main.o
TARGET=main

.PHONY: everything clean all

everything: $(TARGET)

clean:
	rm -fv $(OBJ) $(TARGET)

all: clean everything

food.o: food.cpp food.h
	$(ASM) -c $< -o $@

main.o: main.cpp
	$(ASM) -c $< -o $@

main: $(OBJ)
	$(ASM) $(OBJ) -o $@
