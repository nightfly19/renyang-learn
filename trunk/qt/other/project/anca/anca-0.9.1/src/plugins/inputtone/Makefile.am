METASOURCES = AUTO
pluginsdir = $(pkgdatadir)/plugins
plugins_LTLIBRARIES = libinputtone.la
libinputtone_la_LDFLAGS = -avoid-version -module
libinputtone_la_SOURCES = dialwidget.ui inputtoneplugin.cpp image_collection.cpp

image_collection.cpp: dialer.png
	$(UIC)  -embed inputtone -o $@ $(srcdir)/dialer.png

LIBRARY_MAKEFILE:=1

include $(OPENH323DIR)/openh323u.mak

# set the include path found by configure
INCLUDES = -I../../libcore $(all_includes)

KDE_OPTIONS = qtonly
AM_CXXFLAGS = $(STDCCFLAGS) -I$(PWLIBDIR)/include

DISTCLEANFILES = image_collection.cpp
