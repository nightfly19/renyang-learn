# Program, ASM, ASMFLAGS, etc

ASM = g++
ASMFLAGS = -Wall -g -O3

TARGET = main
OBJ = main.o PointDemo.o Point.o

everything:$(TARGET)

clean:
	rm -fv $(OBJ)

clean-target:
	rm -fv $(TARGET) $(OBJ)

all:clean everything

main.o:main.cpp
	$(ASM) $(ASMFLAGS) -c $< -o $@

PointDemo.o:PointDemo.cpp
	$(ASM) $(ASMFLAGS) -c $< -o $@

Point.o:Point.cpp
	$(ASM) $(ASMFLAGS) -c $< -o $@

main:$(OBJ)
	$(ASM) $(ASMFLAGS) $(OBJ) -o $(TARGET)
	
