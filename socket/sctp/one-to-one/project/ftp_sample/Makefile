LIBS= -lsctp -pthread

TARGET	= cbr

OBJ	= cbr.o

MOD_FLAG = -DUN_MOD

ifeq ($(UN_MOD),off)
	MOD_FLAG =
endif

.PHONY: everything all clean distclean

everything: $(TARGET)

all: distclean everything

clean:
	rm -fv $(OBJ)

distclean: clean
	rm -fv $(TARGET)

cbr.o:cbr.c
	$(CC) $(MOD_FLAG) -c $< -o $@

cbr:cbr.o
	$(CC) $(LIBS) $< -o $@
