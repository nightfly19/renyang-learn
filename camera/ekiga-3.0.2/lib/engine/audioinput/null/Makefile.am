noinst_LTLIBRARIES = libgmaudioinput-null.la

audioinput_dir = $(top_srcdir)/lib/engine/audioinput/null

AM_CXXFLAGS = $(SIGC_CFLAGS) $(GLIB_CFLAGS) $(PTLIB_CFLAGS)

INCLUDES = \
	-I$(top_srcdir)/lib/gmconf \
	-I$(top_srcdir)/lib/engine/include \
	-I$(top_srcdir)/lib/engine/framework \
	-I$(top_srcdir)/lib/engine/audiooutput/skel \
	-I$(top_srcdir)/lib/engine/audioinput/skel \
	-I$(top_srcdir)/lib/engine/audioinput/null \
	-I$(top_srcdir)/lib/engine/hal/skel

libgmaudioinput_null_la_SOURCES = \
	$(audioinput_dir)/audioinput-manager-null.h \
	$(audioinput_dir)/audioinput-manager-null.cpp \
	$(audioinput_dir)/audioinput-main-null.h \
	$(audioinput_dir)/audioinput-main-null.cpp 

libgmaudioinput_null_la_LDFLAGS = -export-dynamic -no-undefined $(SIGC_LIBS) $(GLIB_LIBS) $(PTLIB_LIBS)
