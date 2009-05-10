noinst_LTLIBRARIES = libgmvideooutput-dx.la

videooutput_dir = $(top_srcdir)/lib/engine/videooutput/dx
common_dir = $(top_srcdir)/lib/engine/videooutput/common

AM_CXXFLAGS = $(SIGC_CFLAGS) $(PTLIB_CFLAGS) $(DX_CFLAGS) $(GLIB_CFLAGS)

INCLUDES = \
	-I$(top_srcdir)/lib/gmconf \
	-I$(top_srcdir)/lib/engine/include \
	-I$(top_srcdir)/lib/engine/framework \
	-I$(top_srcdir)/lib/engine/videooutput/skel \
	-I$(top_srcdir)/lib/engine/videooutput/common \
	-I$(top_srcdir)/lib/gui

libgmvideooutput_dx_la_SOURCES = \
	$(common_dir)/videooutput-manager-common.cpp\
	$(common_dir)/videooutput-manager-common.h  \
	$(videooutput_dir)/videooutput-manager-dx.cpp    \
	$(videooutput_dir)/videooutput-manager-dx.h      \
	$(videooutput_dir)/videooutput-main-dx.cpp    \
	$(videooutput_dir)/videooutput-main-dx.h

libgmvideooutput_dx_la_LDFLAGS = -export-dynamic -no-undefined $(SIGC_LIBS) $(PTLIB_LIBS) $(DX_LIBS) $(GLIB_LIBS)
