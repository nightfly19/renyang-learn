# Program, ASM, ASMFLAGS, etc


ASM = g++
ASMFLAGS = -Wall -g -O3

TARGET = copy
OBJ = main.o

.PHONY:everything all clean distclean

everything:$(TARGET)

all:distclean everything

clean:
	rm -fv $(OBJ)

distclean:
	rm -fv $(OBJ) $(TARGET)

main.o:main.cpp
	$(ASM) $(ASMFLAGS) -c $< -o $@

copy:$(OBJ)
	$(ASM) $(ASMFLAGS) $(OBJ) -o $@
