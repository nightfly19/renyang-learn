METASOURCES = AUTO
pluginsdir = $(pkgdatadir)/plugins
plugins_LTLIBRARIES = libvideoout.la
libvideoout_la_LDFLAGS = -avoid-version -module
libvideoout_la_SOURCES = configdialog.ui framebuffer.cpp videooutplugin.cpp videowidget.cpp yuv2rgb.cpp

LIBRARY_MAKEFILE:=1

include $(OPENH323DIR)/openh323u.mak

# set the include path found by configure
INCLUDES = -I../../libcore $(all_includes)

KDE_OPTIONS = qtonly
AM_CXXFLAGS = $(STDCCFLAGS) -I$(PWLIBDIR)/include
