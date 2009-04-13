CC	= gcc

TARGET	= client server

OBJ	= client.o server.o

MAINPATH= ../../../../..

CFLAGS	= -I$(MAINPATH)/lib

LIBS	= -lsctp

CUSTOMER	= $(MAINPATH)/lib/libunp.a

.PHONY: everything all clean distclean

everything: $(TARGET)

all: distclean everything

clean:
	rm -fv $(OBJ)

# 同時移除掉lib的Makefile所產生的檔案
distclean: clean
	rm -fv $(TARGET)
	$(MAKE) distclean -C $(MAINPATH)/lib

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

server:server.o $(CUSTOMER)
	$(CC) $(LIBS) $< $(CUSTOMER) -o $@

client:client.o $(CUSTOMER)
	$(CC) $(LIBS) $< $(CUSTOMER) -o $@

# 只有當有定義GETPRIM=on時,client端在執行時才會出現目前的association的要傳送到對方的主要primary address
ifeq ($(GETPRIM),off)
$(CUSTOMER):
	$(MAKE) -C $(MAINPATH)/lib
else
$(CUSTOMER):
	$(MAKE) GETPRIM=on -C $(MAINPATH)/lib
endif
