
ASM		= gcc

SCTP_FLAG	= -lsctp

OBJ		= serv.o clnt.o

TARGET		= serv clnt

.PHONY: everything clean distclean all

everything: $(TARGET)

clean:
	rm -vf $(OBJ)

distclean:clean
	rm -fv $(TARGET)

all: distclean everything

serv.o:serv.c
	$(ASM) -c $< -o $@

serv: serv.o
	$(ASM) $(SCTP_FLAG) $< -o $@

clnt.o:clnt.c
	$(ASM) -c $< -o $@

clnt:clnt.o
	$(ASM) $(SCTP_FLAG) $< -o $@
