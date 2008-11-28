# Program, ASM, ASMFLAGS, etc

ASM = g++
ASMFLAGS = -Wall -g -O3

TARGET = main
OBJ = main.o Integer.o

.PHONY:everything all clean clean_target

everything:$(TARGET)

clean:
	rm -fv $(OBJ)

clean_target:
	rm -fv $(OBJ) $(TARGET)

all:clean_target everything

main.o:main.cpp
	$(ASM) $(ASMFLAGS) -c $< -o $@

Integer.o:Integer.cpp Integer.h
	$(ASM) $(ASMFLAGS) -c $< -o $@

main:$(OBJ)
	$(ASM) $(ASMFLAGS) $(OBJ) -o $(TARGET)
