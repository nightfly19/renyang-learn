
ASM	= gcc

OBJ	= udpclient.o udpserver.o

TARGET	= udpclient udpserver

.PHONY: everything all clean distclean

everything:$(TARGET)

all: distclean everything

clean:
	rm -fv $(OBJ)

distclean:clean
	rm -fv $(TARGET)

udpclient.o:udpclient.c
	$(ASM) -c $< -o $@

udpserver.o:udpserver.c
	$(ASM) -c $< -o $@

udpclient:udpclient.o
	$(ASM) $< -o $@

udpserver:udpserver.o
	$(ASM) $< -o $@
