CC	= gcc

OBJ	= hello.o world.o

LIBS	= libmylib.so.1.0.0 libmylib.so libmylib.so.1

TARGET	= main

.PHONY: everything all clean distclean

everything: $(TARGET)

all: distclean everything

clean:
	rm -fv $(OBJ)

distclean: clean
	rm -fv $(TARGET) $(LIBS)

hello.o:hello.c
	$(CC) -c -fPIC $< -o $@

world.o:world.c
	$(CC) -c -fPIC $< -o $@

libmylib.so.1.0.0:$(OBJ)
	$(CC) -shared -Wl,-soname,libmylib.so.1 -o $@ $(OBJ)

libmylib.so:libmylib.so.1.0.0
	ln -s $< $@

libmylib.so.1:libmylib.so.1.0.0
	ln -s $< $@

main: main.c $(LIBS)
	$(CC) $< -L. -lmylib -o $@
