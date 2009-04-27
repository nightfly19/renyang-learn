METASOURCES = AUTO
pluginsdir = $(pkgdatadir)/plugins
plugins_LTLIBRARIES = libvideoin.la
libvideoin_la_LDFLAGS = -avoid-version -module
libvideoin_la_SOURCES = configdialog.ui videograbber.cpp videoinplugin.cpp fakedevice.cpp

LIBRARY_MAKEFILE:=1

include $(OPENH323DIR)/openh323u.mak

# set the include path found by configure
INCLUDES = -I../../libcore $(all_includes)

KDE_OPTIONS = qtonly
AM_CXXFLAGS = $(STDCCFLAGS) -I$(PWLIBDIR)/include
