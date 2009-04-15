
CC	= gcc

OPTION	= -fopenmp

OBJ	= RankNum.o
TARGET	= RankNum

.PHONY: everything all clean distclean

everything:$(TARGET)

all: distclean everything

clean:
	rm -fv $(OBJ)

distclean: clean
	rm -fv $(TARGET)

RankNum.o:RankNum.c
	$(CC) $(OPTION) -c $< -o $@

RankNum:RankNum.o
	$(CC) $(OPTION) $< -o $@
