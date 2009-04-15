# Program, ASM, ASMFLAGS, etc

ASM = g++
ASMFLAGS = -Wall -g -O3

TARGET = main
OBJ = main.o

.PHONY:everything all clean distclean

everything:$(TARGET)

all:distclean everything

clean:
	rm -fv $(OBJ)

distclean:
	rm -fv $(OBJ) $(TARGET)

main.o:main.cpp Point2D.h Point3D.h
	$(ASM) $(ASMFLAGS) -c $< -o $@

main:$(OBJ)
	$(ASM) $(ASMFLAGS) $(OBJ) -o $@
