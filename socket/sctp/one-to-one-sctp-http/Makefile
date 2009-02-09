cc=gcc
cflags_nsl=-lnsl
cflags_thread=-lpthread
cflags_sem=-lrt
cflags_sctp=-lsctp

.PHONY: all clean

all:
	$(cc) $(cflags_nsl) $(cflags_sctp) client.c -o cli/client.o
	$(cc) $(cflags_thread) $(cflags_nsl) $(cflags_sem) $(cflags_sctp) server.c -o ser/server.o

client:
	$(cc) $(cflags_nsl) $(cflags_sctp) client.c -o cli/client.o

server:
	$(cc) $(cflags_thread) $(cflags_nsl) $(cflags_sem) $(cflags_sctp) server.c -o ser/server.o

clean:
	rm -f cli/*.o
	rm -r ser/*.o

