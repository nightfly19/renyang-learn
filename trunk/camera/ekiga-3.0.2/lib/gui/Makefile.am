SUBDIRS = 

noinst_LTLIBRARIES = libgmwidgets.la

libgmwidgets_la_SOURCES =	\
	gm-smileys.h		\
	gm-smileys.c		\
	gmwindow.c		\
	gmwindow.h		\
	gmconfwidgets.c		\
	gmconfwidgets.h		\
	codecsbox.cpp		\
	codecsbox.h		\
	gmconnectbutton.c	\
	gmconnectbutton.h	\
	gmdialog.c		\
	gmdialog.h		\
	gmentrydialog.c		\
	gmentrydialog.h		\
	gmlevelmeter.c		\
	gmlevelmeter.h		\
	gmmenuaddon.c		\
	gmmenuaddon.h		\
	gmpreferences.c		\
	gmpreferences.h		\
	gmstatusbar.c		\
	gmstatusbar.h		\
	gmstockicons.c		\
	gmstockicons.h		\
	gmpowermeter.c		\
	gmpowermeter.h		\
	gmcellrendererexpander.c  \
	gmcellrendererexpander.h  \
	gm-cell-renderer-bitext.c \
	gm-cell-renderer-bitext.h \
	gm-text-buffer-enhancer-helper-iface.c \
	gm-text-buffer-enhancer-helper-iface.h \
	gm-text-anchored-tag.c \
	gm-text-anchored-tag.h \
	gm-text-smiley.c \
	gm-text-smiley.h \
	gm-text-extlink.c \
	gm-text-extlink.h \
	gm-text-buffer-enhancer.c \
	gm-text-buffer-enhancer.h \
	gm-smiley-chooser-button.c \
	gm-smiley-chooser-button.h


if !WIN32
libgmwidgets_la_SOURCES +=	\
	xwindow.cpp		\
	xwindow.h
endif

if HAVE_XV
libgmwidgets_la_SOURCES +=	\
	xvwindow.cpp		\
	xvwindow.h
endif

if HAVE_DX
libgmwidgets_la_SOURCES +=	\
	dxwindow.cpp		\
	dxwindow.h
endif

INCLUDES =						\
	-I$(top_srcdir)/lib/gmconf			\
	-I$(top_srcdir)/lib				\
	-I$(top_srcdir)/lib/engine/protocol/skel	\
	-I$(top_srcdir)					\
	-I$(top_builddir)

if !WIN32
INCLUDES +=						\
	-I$(top_srcdir)/lib/pixops
endif

AM_CXXFLAGS = $(GTK_CFLAGS) $(GLIB_CFLAGS) $(OPAL_CFLAGS) $(PTLIB_CFLAGS) $(XV_CFLAGS) $(DX_CFLAGS) $(SIGC_CFLAGS) -I ../../lib/engine/protocol/skel/ -I ../../lib/engine/framework/
AM_CFLAGS = $(GTK_CFLAGS) $(GLIB_CFLAGS) $(GNOME_CFLAGS)
AM_LIBS = $(GTK_LIBS) $(GLIB_LIBS) $(OPAL_LIBS) $(PTLIB_LIBS) $(XV_LIBS) $(DX_LIBS) $(GNOME_LIBS) $(SIGC_LIBS)

libgmwidgets_la_LDFLAGS = -export-dynamic
libgmwidgets_la_LIBADD = $(AM_LIBS)
