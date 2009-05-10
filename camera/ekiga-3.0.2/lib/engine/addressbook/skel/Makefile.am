noinst_LTLIBRARIES = libgmaddressbook.la

addressbook_dir = $(top_srcdir)/lib/engine/addressbook/skel

AM_CXXFLAGS = $(SIGC_CFLAGS)

INCLUDES = \
	-I$(top_srcdir)/lib/engine/include \
	-I$(top_srcdir)/lib/engine/framework

libgmaddressbook_la_SOURCES = \
	$(addressbook_dir)/contact.h			\
	$(addressbook_dir)/book.h			\
	$(addressbook_dir)/book-impl.h		\
	$(addressbook_dir)/source.h			\
	$(addressbook_dir)/source-impl.h		\
	$(addressbook_dir)/contact-core.h		\
	$(addressbook_dir)/contact-core.cpp

libgmaddressbook_la_LDFLAGS = -export-dynamic -no-undefined $(SIGC_LIBS)
