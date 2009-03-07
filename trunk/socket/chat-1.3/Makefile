VERSION =	1.3

INCLUDE =	-I/usr/local/include/Sockets 
CFLAGS =	-Wall -g -O2 $(INCLUDE) -MD `Sockets-config`
CPPFLAGS =	$(CFLAGS)
LIBS =		-L/usr/local/lib -lSockets -lpthread -lssl -lcrypto

#CFLAGS +=	-DMACOSX
#CFLAGS +=	-DSOLARIS

PROGS =		chat

all:		$(PROGS)

CHAT =		ChatSocket.o chat.o ChatHandler.o 
chat:		$(CHAT)
		g++ -o $@ $^ $(LIBS)

clean:
		rm -f *.o *~ *.d slask $(PROGS)

install:	all
		install --strip $(PROGS) /usr/local/bin

-include	*.d

docs:
		/usr/local/bin/dp -dot -cluster | grep -v std | dot -Tpng -o /usr/local/apache/www.alhem.net/htdocs/dot/chat.png
		/usr/bin/doxygen doxygen.cfg

PROJ_NAME =	chat
PROJ_DIR =	/usr/local/apache/www.alhem.net/htdocs/project/example3
tar:		clean
		tar czf $(PROJ_NAME)-$(VERSION).tar.gz *.h *.cpp Makefile \
			gpl.txt
		/usr/local/bin/tarfix.sh $(PROJ_NAME)-$(VERSION)
		cp $(PROJ_NAME)-$(VERSION).tar.gz $(PROJ_DIR)


