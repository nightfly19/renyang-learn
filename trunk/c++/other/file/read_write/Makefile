# Program, ASM, ASMFLAGS, etc

ASM = g++
ASMFLAGS = -Wall -g -O3

TARGET = write read
OBJ1 = write.o
OBJ2 = read.o

.PHONY:everything all clean clean_target

everything:$(TARGET)

all: clean_target everything

clean_target:
	rm -fv $(OBJ1) $(OBJ2) $(TARGET)

clean:
	rm -fv $(OBJ1) $(OBJ2)

write.o:write.cpp
	$(ASM) $(ASMFLAGS) -c $< -o $@

read.o:read.cpp
	$(ASM) $(ASMFLAGS) -c $< -o $@

read:read.o
	$(ASM) $(ASMFLAGS) $< -o $@

write:write.o
	$(ASM) $(ASMFLAGS) $< -o $@
