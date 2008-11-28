# Program, ASM, ASMFLAGS, etc.

ASM = g++
ASMFLAGS = -Wall -g -O3

TARGET = main
OBJ = main.o Point2D.o

.PHONY:everything all clean clean_target

everything:$(TARGET)

clean:
	rm -fv $(OBJ)

clean_target:
	rm -fv $(OBJ) $(TARGET)

all:clean_target everything

main.o:main.cpp
	$(ASM) $(ASMFLAGS) -c $< -o $@

Point2D.o:Point2D.cpp Point2D.h
	$(ASM) $(ASMFLAGS) -c $< -o $@

main:$(OBJ)
	$(ASM) $(ASMFLAGS) $(OBJ) -o $@
