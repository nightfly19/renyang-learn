# Program, ASM, ASMFLAGS, etc


ASM = g++
ASMFLAGS = -Wall -g -O3

TARGET = copy
OBJ = main.o

.PHONY:everything all clean clean_target

everything:$(TARGET)

all:clean_target everything

clean:
	rm -fv $(OBJ)

clean_target:
	rm -fv $(OBJ) $(TARGET)

main.o:main.cpp
	$(ASM) $(ASMFLAGS) -c $< -o $@

copy:$(OBJ)
	$(ASM) $(ASMFLAGS) $(OBJ) -o $@
