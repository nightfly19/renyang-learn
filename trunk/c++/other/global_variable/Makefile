
CC	= gcc
CXX	= g++
CFLAGS	= -Wall -g -O3

# ==========================
OBJ	= main.o
TARGET	= main

.PHONY:everything all clean distclean

everything:$(TARGET)

all:distclean everything

clean:
	rm -fv $(OBJ)

distclean:clean
	rm -fv $(TARGET)

main:$(OBJ)
	$(CXX) $(CFLAGS) $(OBJ) -o $@

main.o:main.cpp
	$(CXX) $(CFLAGS) -c $< -o $@
