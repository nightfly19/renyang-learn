METASOURCES = AUTO

lib_LTLIBRARIES = libancacore.la

libancacore_la_SOURCES = ancaconf.cpp ancainfo.cpp plugin.cpp pluginfactory.cpp

LIBRARY_MAKEFILE:=1

include $(OPENH323DIR)/openh323u.mak

# set the include path found by configure
INCLUDES = $(all_includes)

# the library search path.
libancacore_la_LIBADD = $(LIB_QT) $(LDLIBS)
libancacore_la_LDFLAGS = -no-undefined $(all_libraries) $(QT_LDFLAGS)

AM_CXXFLAGS = $(STDCCFLAGS) -I$(PWLIBDIR)/include

KDE_OPTIONS = qtonly
