INCLUDES = \
	-I$(top_srcdir)/lib				\
	-I$(top_srcdir)/lib/gmconf			\
	-I$(top_srcdir)/lib/toolbox			\
	-I$(top_srcdir)/lib/gui				\
	-I$(top_srcdir)/lib/engine/			\
	-I$(top_srcdir)/lib/engine/gui/gtk-frontend	\
	-I$(top_srcdir)/lib/engine/account/skel		\
	-I$(top_srcdir)/lib/engine/addressbook/skel	\
	-I$(top_srcdir)/lib/engine/chat/skel		\
	-I$(top_srcdir)/lib/engine/presence/skel	\
	-I$(top_srcdir)/lib/engine/protocol/skel	\
	-I$(top_srcdir)/lib/engine/protocol/sip		\
	-I$(top_srcdir)/lib/engine/videooutput/skel	\
	-I$(top_srcdir)/lib/engine/videoinput/skel	\
	-I$(top_srcdir)/lib/engine/audioinput/skel	\
	-I$(top_srcdir)/lib/engine/audiooutput/skel	\
	-I$(top_srcdir)/lib/engine/hal/skel		\
	-I$(top_srcdir)/lib/engine/framework		\
	-I$(top_srcdir)/lib/engine/gui/gtk-core		\
	-I$(top_srcdir)/src				\
	-I$(top_srcdir)/src/clients/			\
	-I$(top_srcdir)/src/components/			\
	-I$(top_srcdir)/src/devices/			\
	-I$(top_srcdir)/src/endpoints/			\
	-I$(top_srcdir)/src/gui/			\
	-I$(top_builddir)				\
	$(GNOME_INCLUDEDIR)				\
	-DSCHEMA_AGE=@SCHEMA_AGE@ 

BUILT_SOURCES = src/revision.h

# XVideo/X
if !WIN32
INCLUDES +=                   				\
	-I$(top_srcdir)/lib/engine/videooutput/common   \
	-I$(top_srcdir)/lib/engine/videooutput/x
endif

# DirectX
if HAVE_DX
INCLUDES +=                   				\
	-I$(top_srcdir)/lib/engine/videooutput/common   \
	-I$(top_srcdir)/lib/engine/videooutput/dx
endif


#-DG_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED -DGDK_DISABLE_DEPRECATED -DGDK_DISABLE_DEPRECATED -DGDK_PIXBUF_DISABLE_DEPRECATED -DGNOME_DISABLE_DEPRECATED

bin_SCRIPTS = ekiga-config-tool
bin_PROGRAMS = ekiga

EXTRA_PROGRAMS =

# Common stuff
ekiga_SOURCES =			\
	common.h		

# Graphical User Interface 
ekiga_SOURCES +=			\
	gui/accounts.h    		\
	gui/accounts.cpp		\
	gui/callbacks.h			\
	gui/callbacks.cpp		\
	gui/conf.h			\
	gui/conf.cpp			\
	gui/dialpad.h			\
	gui/dialpad.cpp			\
	gui/assistant.h			\
	gui/assistant.cpp		\
	gui/main.h			\
	gui/main.cpp			\
	gui/misc.h			\
	gui/misc.cpp			\
	gui/preferences.h		\
	gui/preferences.cpp		\
	gui/statusicon.h		\
	gui/statusicon.cpp		\
	gui/statusmenu.h		\
	gui/statusmenu.cpp

# Devices 
ekiga_SOURCES +=			\
	devices/videoinput.h  	        \
	devices/videoinput.cpp		\
	devices/videooutput.h           \
	devices/videooutput.cpp         \
	devices/audiodev.h              \
	devices/audiodev.cpp


# Endpoints 
ekiga_SOURCES +=				\
	endpoints/ekiga.h			\
	endpoints/ekiga.cpp			\
	endpoints/manager.h			\
	endpoints/manager.cpp			\
	endpoints/pcss.h			\
	endpoints/pcss.cpp			\
	endpoints/opal-account.h		\
	endpoints/opal-account.cpp		\
	endpoints/opal-bank.h			\
	endpoints/opal-bank.cpp			\
	endpoints/opal-call.h			\
	endpoints/opal-call.cpp			\
	endpoints/opal-codec-description.h	\
	endpoints/opal-codec-description.cpp	\
	endpoints/opal-gmconf-bridge.h		\
	endpoints/opal-gmconf-bridge.cpp	\
	endpoints/opal-main.h			\
	endpoints/opal-main.cpp

if HAVE_H323
ekiga_SOURCES +=		\
	endpoints/h323-endpoint.h		\
	endpoints/h323-endpoint.cpp
endif

if HAVE_SIP
ekiga_SOURCES +=		\
	endpoints/sip-chat-simple.h		\
	endpoints/sip-chat-simple.cpp		\
	endpoints/sip-dialect.h			\
	endpoints/sip-dialect.cpp		\
	endpoints/sip-endpoint.h		\
	endpoints/sip-endpoint.cpp
endif

if HAVE_DBUS 
ekiga_SOURCES +=		\
	components/dbus-stub.h	\
	components/dbus.h	\
	components/dbus.cpp


bin_PROGRAMS += ekiga-helper

ekiga_helper_SOURCES = 		\
	components/dbus-helper-stub.h 	\
	components/dbus-helper.cpp

ekiga_helper_LDADD = $(DBUS_LIBS)

BUILT_SOURCES += components/dbus-helper-stub.h components/dbus-stub.h

components/dbus-helper-stub.h: components/dbus-helper-stub.xml
	$(LIBTOOL) --mode=execute dbus-binding-tool --prefix=helper --mode=glib-server --output=$@ $^

components/dbus-stub.h: components/dbus-stub.xml
	$(LIBTOOL) --mode=execute dbus-binding-tool --prefix=ekiga_dbus_component --mode=glib-server --output=$@ $^
endif

if INSTALL_DBUS_SERVICE

%.service: %.service.in
	$(mkdir_p) components # this is ugly...
	sed -e "s#\@bindir\@#@bindir@#" $< > $@

servicedir = $(datadir)/dbus-1/services
service_DATA = components/org.ekiga.Ekiga.service components/org.ekiga.Helper.service
service_in_files = $(service_DATA:.service=.service.in)

endif

SVN_REVISION=$(shell head ../.svn/entries -n11 2>/dev/null | tail -n1)
CACHED_REVISION=$(shell cat revision.h 2>/dev/null | cut -c24-)

src/revision.h:
$(shell if test -e "../.svn/entries"; then \
          if test "x$(SVN_REVISION)" != "x$(CACHED_REVISION)"; then \
            echo "#define EKIGA_REVISION $(SVN_REVISION)" > revision.h; \
          fi \
        else \
          if !(test -e "./revision.h"); then\
            echo "#define EKIGA_REVISION \"unknown\"" > revision.h; \
          fi \
        fi)

AM_CXXFLAGS = $(GTK_CFLAGS) $(GLIB_CFLAGS) $(GNOME_CFLAGS) $(DBUS_CFLAGS) $(BONOBO_CFLAGS) $(OPAL_CFLAGS) $(PTLIB_CFLAGS) $(SIGC_CFLAGS) $(XML_CFLAGS) $(NOTIFY_CFLAGS)
AM_LIBS = $(GTK_LIBS) $(GLIB_LIBS) $(GNOME_LIBS) $(DBUS_LIBS) $(BONOBO_LIBS) $(OPAL_LIBS) $(PTLIB_LIBS) $(SIGC_LIBS) $(XML_LIBS) $(NOTIFY_LIBS)

ekiga_LDADD = \
	$(top_builddir)/lib/libekiga.la $(top_builddir)/lib/engine/libekiga_engine.la $(AM_LIBS) 

EXTRA_DIST = \
	$(service_in_files)		\
	components/dbus-stub.xml	\
	components/dbus-helper-stub.xml

CLEANFILES = \
	$(service_DATA)

DISTCLEANFILES = revision.h
