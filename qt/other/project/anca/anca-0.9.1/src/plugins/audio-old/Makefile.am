METASOURCES = AUTO
pluginsdir = $(pkgdatadir)/plugins
plugins_LTLIBRARIES = libaudio.la
libaudio_la_LDFLAGS = -avoid-version -module
libaudio_la_SOURCES = audiocodecswidget.ui configdialog.ui audioplugin.cpp audioconfigwidget.cpp

LIBRARY_MAKEFILE:=1

include $(OPENH323DIR)/openh323u.mak

# set the include path found by configure
INCLUDES = -I../../libcore $(all_includes)

KDE_OPTIONS = qtonly
AM_CXXFLAGS = $(STDCCFLAGS) -I$(PWLIBDIR)/include
