CC	= gcc

LIBS	= libmylib.a

TARGET	= main

OBJ	= hello.o world.o

.PHONY: everything all clean distclean

everything:$(TARGET)

all: distclean everything

clean:
	rm -fv $(OBJ)

distclean: clean
	rm -fv $(LIBS) $(TARGET)

# 產生執行檔,且使用到我們自己產生的static library
main:main.c $(LIBS)
	$(CC) $< -L. -lmylib -o $@

# 產生static library
libmylib.a:hello.o world.o
	ar rcs libmylib.a hello.o world.o

# 產生object file
hello.o: hello.c
	$(CC) -c $<

# 產生object file
world.o: world.c
	$(CC) -c $<
