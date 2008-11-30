# Program, ASM, ASMFLAGS, etc.

ASM = g++
ASMFLAGS = -Wall -g -O3

TARGET = read
OBJ = main.o

.PHONY:everything all clean clean_target

everything:$(TARGET)

clean:
	rm -fv $(OBJ)

clean_target:
	rm -fv $(OBJ) $(TARGET)

all:clean_target everything

main.o:main.cpp
	$(ASM) $(ASMFLAGS) -c $< -o $@

read:$(OBJ)
	$(ASM) $(ASMFLAGS) $(OBJ) -o $@
