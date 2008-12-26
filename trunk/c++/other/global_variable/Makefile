
CC	= gcc
CXX	= g++
CFLAGS	= -Wall -g -O3

# ==========================
OBJ	= main.o
TARGET	= main

.PHONY:everything all clean clean_target

everything:$(TARGET)

all:clean_target everything

clean:
	rm -fv $(OBJ)

clean_target:clean
	rm -fv $(TARGET)

main:$(OBJ)
	$(CXX) $(CFLAGS) $(OBJ) -o $@

main.o:main.cpp
	$(CXX) $(CFLAGS) -c $< -o $@
