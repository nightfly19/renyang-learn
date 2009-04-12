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

# 設定sctp_strcli.c內的下面兩個巨集內的程式碼打開,由這裡控制lib/Makefile
# 再由lib/Makefile去定義GETPRIM與SETPRIM這兩個巨集
$(CUSTOMER):
	$(MAKE) GETPRIM=on SETPRIM=on -C $(MAINPATH)/lib
