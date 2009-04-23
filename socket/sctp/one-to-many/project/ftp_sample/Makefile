LIBS= -lsctp -pthread

TARGET	= cbr

OBJ	= cbr.o

.PHONY: everything all clean distclean

everything: $(TARGET)

all: distclean everything

clean:
	rm -fv $(OBJ)

distclean: clean
	rm -fv $(TARGET)

cbr.o:cbr.c
	$(CC) -c $< -o $@

cbr:cbr.o
	$(CC) $(LIBS) $< -o $@
