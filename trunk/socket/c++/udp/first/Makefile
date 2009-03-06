
CXX	= g++

OBJ	= main.o

TARGET	= main

.PHONY: everything all clean clean_target

everything: $(TARGET)

all: clean_target everything

clean:
	rm -fv $(OBJ)

clean_target: clean
	rm -fv $(TARGET)

main.o:main.cpp
	$(CXX) -c $< -o $@

main:main.o
	$(CXX) $< -o $@
