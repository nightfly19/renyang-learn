CC	= gcc

OBJ	= client.o

MAINPATH= ../../../../

LIBS	= $(MAINPATH)/lib/libunp.a

CFLAGS	= -I$(MAINPATH)/lib

TARGET	= client

.PHONY: everything all clean distclean

everything: $(TARGET)

all: distclean everything

clean:
	rm -fv $(OBJ)

distclean: clean
	rm -fv $(TARGET)

client.o:client.c
	$(CC) -c $< -o $@ $(CFLAGS)

# 建立可執行檔需要建立static library
client:client.o $(LIBS)
	$(CC) $< -o $@ $(LIBS)

# 建立需要的static library
# -C是切換目錄
# -M是回到指定的目錄
$(LIBS):
	$(MAKE) -w -C $(MAINPATH)/lib M=$(PWD)
