CROSS_COMPILE = arm-unknown-linux-gnu-gcc

CC=$(CROSS_COMPILE) $(CFLAGS) -I $(LINUXDIR)/include
LINUXDIR = /home/acx/lab3/microtime/linux

EXEC = listen
OBJS = seriallisten.o proj.o
LIBS = -L. -lmote

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) -lpthread  $(OBJS) $(LIBS) -o $@

romfs:
	$(ROMFSINST) /bin/$(EXEC)

clean:
	-rm -f $(EXEC) *.elf *.gdb $(OBJS)

