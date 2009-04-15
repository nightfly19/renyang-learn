
.PHONY: everything all clean distclean

everything:
	make -f makefile.server
	make -f makefile.client

all: distclean everything

clean:
	make -f makefile.server clean
	make -f makefile.client clean

distclean:
	make -f makefile.server distclean
	make -f makefile.client distclean
