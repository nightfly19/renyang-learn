CC	= gcc

TARGET	= server

OBJ	= server.o

MAINPATH= ../..

CFLAGS	= -I$(MAINPATH)/lib

CUSTOMER	= $(MAINPATH)/lib/libunp.a

.PHONY: everything all clean distclean

everything: $(TARGET)

all: distclean everything

clean:
	rm -fv $(OBJ)

distclean: clean
	rm -fv $(TARGET)
	$(MAKE) $@ -C $(MAINPATH)/lib

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

server:server.o $(CUSTOMER)
	$(CC) $(LIBS) $< $(CUSTOMER) -o $@

$(CUSTOMER):
	$(MAKE) -C $(MAINPATH)/lib
