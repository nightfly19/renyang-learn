# Program, ASM, ASMFLAGS, etc.

ASM = g++
ASMFLAGS = -Wall -g -O3

OBJ = main.o
TARGET = write

.PHONY:everything clean clean_target all

everything:$(TARGET)

clean:
	rm -fv $(OBJ)

clean_target:
	rm -fv $(OBJ) $(TARGET)

all:clean_target everything

main.o:main.cpp
	$(ASM) $(ASMFLAGS) -c $< -o $@

write:$(OBJ)
	$(ASM) $(ASMFLAGS) $(OBJ) -o $@
