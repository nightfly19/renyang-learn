# Program, ASM, ASMFLAGS,etc

ASM = g++
ASMFLAGS = -Wall -g -O3

TARGET=main
OBJ=main.o Ball.o


.PHONY:everything all clean

everything:$(TARGET)

all:clean everything

clean:
	rm -fv $(TARGET) $(OBJ)

main.o:main.cpp
	$(ASM) $(ASMFLAGS) -c $< -o $@

Ball.o:Ball.cpp Ball.h
	$(ASM) $(ASMFLAGS) -c $< -o $@

main:$(OBJ)
	$(ASM) $(ASMFLAGS) $(OBJ) -o $@
