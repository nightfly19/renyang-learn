
CC	= g++

OBJ	= main.o

TARGET	= server

CXXFLAGS= -I..

LIBS	= -lsctp -lcv -lhighgui

.PHONY: everything all clean distclean

everything: $(TARGET)

all: distclean everything

clean:
	rm -fv $(OBJ)

distclean: clean
	rm -fv $(TARGET)

main.o:main.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@

server:main.o
	$(CC) $(LIBS) $< -o $@
