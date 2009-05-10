noinst_LTLIBRARIES = libgmaudiooutput-ptlib.la

audiooutput_dir = $(top_srcdir)/lib/engine/audiooutput/ptlib

AM_CXXFLAGS = $(SIGC_CFLAGS) $(GLIB_CFLAGS) $(PTLIB_CFLAGS)

INCLUDES = \
	-I$(top_srcdir)/lib/gmconf \
	-I$(top_srcdir)/lib/engine/include \
	-I$(top_srcdir)/lib/engine/framework \
	-I$(top_srcdir)/lib/engine/audiooutput/skel \
	-I$(top_srcdir)/lib/engine/audiooutput/ptlib \
	-I$(top_srcdir)/lib/engine/hal/skel

libgmaudiooutput_ptlib_la_SOURCES = \
	$(audiooutput_dir)/audiooutput-manager-ptlib.h \
	$(audiooutput_dir)/audiooutput-manager-ptlib.cpp \
	$(audiooutput_dir)/audiooutput-main-ptlib.h \
	$(audiooutput_dir)/audiooutput-main-ptlib.cpp 

libgmaudiooutput_ptlib_la_LDFLAGS = -export-dynamic -no-undefined $(SIGC_LIBS) $(GLIB_LIBS) $(PTLIB_LIBS)
