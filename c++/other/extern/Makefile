
# Program, flags, etc.

ASM = g++
ASMFLAGS = -Wall -g -O3
OBJ = main.o some.o
TARGET = main

.PHONY:everything clean all

everything:$(TARGET)

clean:
	rm -fv $(OBJ) $(TARGET)

all: clean everything

main.o:main.cpp
	$(ASM) $(ASMFLAGS) -c $< -o $@

some.o:some.cpp
	$(ASM) $(ASMFLAGS) -c $< -o $@
main:$(OBJ)
	$(ASM) $(ASMFLAGS) $(OBJ) -o $(TARGET)
