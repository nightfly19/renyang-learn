
CC	= gcc

OPTION	= -fopenmp

OBJ	= RankNum.o
TARGET	= RankNum

.PHONY: everything all clean clean_target

everything:$(TARGET)

all: clean_target everything

clean:
	rm -fv $(OBJ)

clean_target: clean
	rm -fv $(TARGET)

RankNum.o:RankNum.c
	$(CC) $(OPTION) -c $< -o $@

RankNum:RankNum.o
	$(CC) $(OPTION) $< -o $@
