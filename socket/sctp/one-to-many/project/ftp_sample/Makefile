LIBS= -lsctp -pthread

TARGET	= cbr

OBJ	= cbr.o

ifeq ($(UN_MOD),on)
	MOD_FLAG = -DUN_MOD
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
