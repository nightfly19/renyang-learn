
ASM = g++
ASMFLAGS = -O3
OBJ=main.o
TARGET=main

.PHONY:everything clean all

everything:$(TARGET)

clean:
	rm -fv $(OBJ) $(TARGET)

all:clean everything

main.o:main.cpp
	$(ASM) $(ASMFLAGS) -c $< -o $@

main:$(OBJ)
	$(ASM) $(ASMFLAGE) $(OBJ) -o $@
