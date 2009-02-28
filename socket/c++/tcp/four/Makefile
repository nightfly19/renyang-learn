
.PHONY: everything all clean clean_target

everything:
	make -f makefile.server
	make -f makefile.client

all: clean_target everything

clean:
	make -f makefile.server clean
	make -f makefile.client clean

clean_target:
	make -f makefile.server clean_target
	make -f makefile.client clean_target
