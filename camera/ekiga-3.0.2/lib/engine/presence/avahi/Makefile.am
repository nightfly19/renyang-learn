noinst_LTLIBRARIES = libgmavahi.la

avahi_dir = $(top_srcdir)/lib/engine/presence/avahi

AM_CPPFLAGS = $(SIGC_CFLAGS) $(AVAHI_CFLAGS)

INCLUDES = \
	-I$(top_srcdir)/lib/engine/framework \
	-I$(top_srcdir)/lib/engine/account/skel \
	-I$(top_srcdir)/lib/engine/presence/skel

libgmavahi_la_SOURCES = \
	$(avahi_dir)/avahi-heap.h \
	$(avahi_dir)/avahi-heap.cpp \
	$(avahi_dir)/avahi-cluster.h \
	$(avahi_dir)/avahi-cluster.cpp \
	$(avahi_dir)/avahi-main.h \
	$(avahi_dir)/avahi-main.cpp

libgmavahi_la_LIBADD = \
	$(top_builddir)/lib/engine/presence/skel/libgmpresence.la

libgmavahi_la_LDFLAGS = -export-dynamic -no-undefined $(SIGC_LIBS) $(AVAHI_LIBS)
