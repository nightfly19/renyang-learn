SUBDIRS = libcore plugins

bin_PROGRAMS = anca
anca_SOURCES = ancaform.ui answercalldlg.ui configuredialog.ui personaldialog.ui anca.cpp \
	endpoint.cpp guimanager.cpp guiproxy.cpp infomanager.cpp main.cpp slavethread.cpp \
	image_collection.cpp

image_collection.cpp: \
		images/configure.png \
		images/maininfo.png \
		images/preview.png \
		images/location_erase.png \
		images/unknown.png \
		images/connected.png \
		images/disconnected.png \
		images/anca.png
	$(UIC)  -embed anca -o $@ $(srcdir)/images/configure.png $(srcdir)/images/maininfo.png $(srcdir)/images/preview.png $(srcdir)/images/location_erase.png $(srcdir)/images/unknown.png $(srcdir)/images/connected.png $(srcdir)/images/disconnected.png $(srcdir)/images/anca.png

LIBRARY_MAKEFILE:=1

include $(OPENH323DIR)/openh323u.mak

# set the include path found by configure
INCLUDES = -Ilibcore $(all_includes)

# the library search path.
anca_LDFLAGS = $(all_libraries) $(QT_LDFLAGS)
anca_LDADD = libcore/libancacore.la $(LIB_QT) $(LDLIBS)

STDCCFLAGS = -DCONFIGDIR=\"$(CONFIGDIR)\"

KDE_OPTIONS = qtonly
AM_CXXFLAGS = $(STDCCFLAGS) -I$(PWLIBDIR)/include
METASOURCES = AUTO

DISTCLEANFILES = image_collection.cpp

soundsdir = $(pkgdatadir)/sounds
sounds_DATA = sounds/ringing.wav
