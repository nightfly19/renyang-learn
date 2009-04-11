CC	= gcc

OBJ	= error.o wrapunix.o tcp_connect.o readn.o wrapsock.o sigchldwaitpid.o pr_cpu_time.o web_child.o tcp_listen.o signal.o readline.o writen.o sctp_pdapircv.o sctp_wrapper.o sctp_strcliecho.o sctp_getnostrm.o sctp_addr_to_associd.o wraplib.o sctp_strcli.o sctp_displayevents.o sock_ntop.o sctp_strcli_un.o sctp_bindargs.o host_serv.o sctp_bindargs.o sctp_strcli_info.o sctp_check_notify.o sctp_print_addrs.o sctp_modify_hb.o str_echo.o sctp_getprim.o

TARGET	= libunp.a

.PHONY: everything all clean distclean

everything: $(TARGET)

all: distclean everything

clean:
	rm -fv $(OBJ)

distclean: clean
	rm -fv $(TARGET)

# 依每一個c為副檔名的建立其物件檔
%.o:%.c
	$(CC) -c $< -o $@

# 建立Static library
libunp.a:$(OBJ)
	ar rcs $@ $(OBJ)
