
ASM		= gcc

SCTP_FLAG	= -lsctp

OBJ		= serv.o clnt.o

TARGET		= serv clnt

.PHONY: everything clean clean_target all

everything: $(TARGET)

clean:
	rm -rf $(OBJ)

clean_target:clean
	rm -rf $(TARGET)

all: clean_target everything

serv.o:serv.c
	$(ASM) -c $< -o $@

serv: serv.o
	$(ASM) $(SCTP_FLAG) $< -o $@

clnt.o:clnt.c
	$(ASM) -c $< -o $@

clnt:clnt.o
	$(ASM) $(SCTP_FLAG) $< -o $@
