noinst_LTLIBRARIES = libgmaudioinput.la

audioinput_dir = $(top_srcdir)/lib/engine/audioinput/skel

AM_CXXFLAGS = $(SIGC_CFLAGS) $(GLIB_CFLAGS) $(PTLIB_CFLAGS)

INCLUDES = \
	-I$(top_srcdir)/lib/gmconf \
	-I$(top_srcdir)/lib/engine/include \
	-I$(top_srcdir)/lib/engine/framework \
	-I$(top_srcdir)/lib/engine/audiooutput/skel \
	-I$(top_srcdir)/lib/engine/audioinput/skel \
	-I$(top_srcdir)/lib/engine/hal/skel

libgmaudioinput_la_SOURCES = \
	$(audioinput_dir)/audioinput-manager.h	\
	$(audioinput_dir)/audioinput-info.h	\
	$(audioinput_dir)/audioinput-core.h	\
	$(audioinput_dir)/audioinput-core.cpp       \
	$(audioinput_dir)/audioinput-gmconf-bridge.h \
	$(audioinput_dir)/audioinput-gmconf-bridge.cpp

libgmaudioinput_la_LDFLAGS = -export-dynamic -no-undefined $(SIGC_LIBS) $(GLIB_LIBS) $(PTLIB_LIBS)
