
CXX	= g++

OBJ	= main.o

TARGET	= main

.PHONY: everything all clean distclean

everything:$(TARGET)

all: distclean everything

clean:
	rm -fv $(OBJ)

distclean:clean
	rm -fv $(TARGET)

$(OBJ).o:main.cpp
	$(CXX) -c $< -o $@

$(TARGET):$(OBJ)
	$(CXX) $< -o $@
