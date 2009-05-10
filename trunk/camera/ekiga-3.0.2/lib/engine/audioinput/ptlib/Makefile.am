noinst_LTLIBRARIES = libgmaudioinput-ptlib.la

audioinput_dir = $(top_srcdir)/lib/engine/audioinput/ptlib

AM_CXXFLAGS = $(SIGC_CFLAGS) $(GLIB_CFLAGS) $(PTLIB_CFLAGS)

INCLUDES = \
	-I$(top_srcdir)/lib/gmconf \
	-I$(top_srcdir)/lib/engine/include \
	-I$(top_srcdir)/lib/engine/framework \
	-I$(top_srcdir)/lib/engine/audiooutput/skel \
	-I$(top_srcdir)/lib/engine/audioinput/skel \
	-I$(top_srcdir)/lib/engine/audioinput/ptlib \
	-I$(top_srcdir)/lib/engine/hal/skel

libgmaudioinput_ptlib_la_SOURCES = \
	$(audioinput_dir)/audioinput-manager-ptlib.h \
	$(audioinput_dir)/audioinput-manager-ptlib.cpp \
	$(audioinput_dir)/audioinput-main-ptlib.h \
	$(audioinput_dir)/audioinput-main-ptlib.cpp 

libgmaudioinput_ptlib_la_LDFLAGS = -export-dynamic -no-undefined $(SIGC_LIBS) $(GLIB_LIBS) $(PTLIB_LIBS)
