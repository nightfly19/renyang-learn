
CC	= gcc

CFLAGS	= `pkg-config --cflags --libs libxine gdk-2.0`

TARGET	= server client

.PHONY: everything all distclean

everything: $(TARGET)

all: distclean everything

distclean:
	rm -fv $(TARGET)

server:vod6_server.c
	$(CC) $(CFLAGS) $< -o $@

client:vod6_client.c
	$(CC) $(CFLAGS) $< -o $@
