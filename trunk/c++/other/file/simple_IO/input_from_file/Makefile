# Program, ASM, ASMFLAGS, etc.

ASM = g++
ASMFLAGS = -Wall -g -O3

TARGET = read
OBJ = main.o

.PHONY:everything all clean distclean

everything:$(TARGET)

clean:
	rm -fv $(OBJ)

distclean:
	rm -fv $(OBJ) $(TARGET)

all:distclean everything

main.o:main.cpp
	$(ASM) $(ASMFLAGS) -c $< -o $@

read:$(OBJ)
	$(ASM) $(ASMFLAGS) $(OBJ) -o $@
