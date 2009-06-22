
GXX	= g++

OBJ	= main.o foo.o

TARGET	= main

.PHONY: everything all clean distclean

everything: $(TARGET)

all: distclean everything

clean:
	rm -fv $(OBJ)

distclean: clean
	rm -fv $(TARGET)

%.o:%.cpp
	$(CXX) -c $< -o $@

main:$(OBJ)
	$(CXX) $(OBJ) -o $@
