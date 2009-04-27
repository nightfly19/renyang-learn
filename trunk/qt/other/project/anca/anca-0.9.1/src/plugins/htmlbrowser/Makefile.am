METASOURCES = AUTO
pluginsdir = $(pkgdatadir)/plugins
plugins_LTLIBRARIES = libhtmlbrowser.la
libhtmlbrowser_la_LDFLAGS = -avoid-version -module
libhtmlbrowser_la_SOURCES = htmlbrowserwidget.ui htmldialog.ui textbrowser.cpp htmlbrowserplugin.cpp netmime.cpp image_collection.cpp

image_collection.cpp: \
		images/reload.png \
		images/key_enter.png \
		images/gohome.png \
		images/back.png \
		images/forward.png \
		images/htmlbrowser.png
	$(UIC)  -embed htmlbrowser -o $@ $(srcdir)/images/reload.png $(srcdir)/images/key_enter.png $(srcdir)/images/gohome.png $(srcdir)/images/back.png $(srcdir)/images/forward.png $(srcdir)/images/htmlbrowser.png

LIBRARY_MAKEFILE:=1

include $(OPENH323DIR)/openh323u.mak

# set the include path found by configure
INCLUDES = -I../../libcore $(all_includes)

KDE_OPTIONS = qtonly
AM_CXXFLAGS = $(STDCCFLAGS) -I$(PWLIBDIR)/include

DISTCLEANFILES = image_collection.cpp
