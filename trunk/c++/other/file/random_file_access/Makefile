# Program, ASM, ASMFLAGS, etc

ASM = g++
ASMFLAGS = -Wall -g -O3

OBJ = create.o write.o read.o
TARGET = create write read

.PHONY:everything all clean distclean

everything:$(TARGET)

clean:
	rm -fv $(OBJ)

distclean:
	rm -fv $(OBJ) $(TARGET)

all:distclean everything

create.o:create.cpp
	$(ASM) $(ASMFLAGS) -c $< -o $@

write.o:write.cpp
	$(ASM) $(ASMFLAGS) -c $< -o $@

read.o:read.cpp
	$(ASM) $(ASMFLAGS) -c $< -o $@

create:create.o
	$(ASM) $(ASMFLAGS) $< -o $@

write:write.o
	$(ASM) $(ASMFLAGS) $< -o $@

read:read.o
	$(ASM) $(ASMFLAGS) $< -o $@
