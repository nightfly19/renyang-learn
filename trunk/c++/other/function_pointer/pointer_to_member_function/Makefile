# Program, ASM,ASMFLAGS, etc.

ASM = g++
ASMFLAGS = -Wall -g -O3

TARGET = main
OBJ = SafeArray.o main.o

.PHONY:everything clean all

everything:$(TARGET)

all:clean_target everything

clean:
	rm -fv $(OBJ)

clean_target:
	rm -fv $(OBJ) $(TARGET)

SafeArray.o:SafeArray.cpp SafeArray.h
	$(ASM) $(ASMFLAGS) -c $< -o $@

main.o:main.cpp
	$(ASM) $(ASMFLAGS) -c $< -o $@

main:$(OBJ)
	$(ASM) $(ASMFLAGS) $(OBJ) -o $(TARGET)
