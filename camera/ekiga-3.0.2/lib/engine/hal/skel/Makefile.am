noinst_LTLIBRARIES = libgmhal.la

hal_dir = $(top_srcdir)/lib/engine/hal/skel

AM_CXXFLAGS = $(SIGC_CFLAGS)

INCLUDES = \
	-I$(top_srcdir)/lib/engine/include \
	-I$(top_srcdir)/lib/engine/framework \
	-I$(top_srcdir)/lib/engine/hal/skel

libgmhal_la_SOURCES = \
	$(hal_dir)/hal-manager.h	\
	$(hal_dir)/hal-core.h		\
	$(hal_dir)/hal-core.cpp

libgmhal_la_LDFLAGS = -export-dynamic -no-undefined $(SIGC_LIBS)
