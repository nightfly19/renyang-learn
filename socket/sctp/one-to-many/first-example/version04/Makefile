
TARGET	= makefile.server makefile.client

.PHONY: everything all clean distclean

everything: $(TARGET)
	make -f makefile.server
	make -f makefile.client

all: $(TARGET)
	make -f makefile.server all
	make -f makefile.client all

clean: $(TARGET)
	make -f makefile.server clean
	make -f makefile.client clean

distclean: $(TARGET)
	make -f makefile.server distclean
	make -f makefile.client distclean
