
ASM	= gcc

OBJ	= udpclient.o udpserver.o

TARGET	= udpclient udpserver

.PHONY: everything all clean clean_target

everything:$(TARGET)

all: clean_target everything

clean:
	rm -fv $(OBJ)

clean_target:clean
	rm -fv $(TARGET)

udpclient.o:udpclient.c
	$(ASM) -c $< -o $@

udpserver.o:udpserver.c
	$(ASM) -c $< -o $@

udpclient:udpclient.o
	$(ASM) $< -o $@

udpserver:udpserver.o
	$(ASM) $< -o $@
