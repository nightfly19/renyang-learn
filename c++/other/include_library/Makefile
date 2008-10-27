
ASM	= g++
ASMFLAGS= -O3 -I library

OBJ=food.o main.o
TARGET=main

.PHONY: everything clean all

everything: $(TARGET)

clean:
	rm -fv $(OBJ) $(TARGET)

all: clean everything

food.o: food.cpp library/food.h
	$(ASM) $(ASMFLAGS) -c $< -o $@

main.o: main.cpp
	$(ASM) $(ASMFLAGS) -c $< -o $@

main: $(OBJ)
	$(ASM) $(OBJ) -o $@
