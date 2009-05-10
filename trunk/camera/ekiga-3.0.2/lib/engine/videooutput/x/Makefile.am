noinst_LTLIBRARIES = libgmvideooutput-x.la

videooutput_dir = $(top_srcdir)/lib/engine/videooutput/x
common_dir = $(top_srcdir)/lib/engine/videooutput/common

AM_CXXFLAGS = $(SIGC_CFLAGS) $(PTLIB_CFLAGS) $(GLIB_CFLAGS) $(XV_CFLAGS)

INCLUDES = \
	-I$(top_srcdir)/lib/gmconf \
	-I$(top_srcdir)/lib/engine/include \
	-I$(top_srcdir)/lib/engine/framework \
	-I$(top_srcdir)/lib/engine/videooutput/skel \
	-I$(top_srcdir)/lib/engine/videooutput/common \
	-I$(top_srcdir)/lib/gui

libgmvideooutput_x_la_SOURCES = \
	$(common_dir)/videooutput-manager-common.cpp \
	$(common_dir)/videooutput-manager-common.h   \
	$(videooutput_dir)/videooutput-manager-x.cpp  \
	$(videooutput_dir)/videooutput-manager-x.h      \
	$(videooutput_dir)/videooutput-main-x.cpp    \
	$(videooutput_dir)/videooutput-main-x.h

libgmvideooutput_x_la_LDFLAGS = -export-dynamic -no-undefined $(SIGC_LIBS) $(PTLIB_LIBS) $(GLIB_LIBS) $(XV_LIBS)
