#!/usr/bin/make -f

SILENT := 0
DEBUG := 1
MAKEFLAGS:=
MAKEOPTS:=$(MAKEFLAGS)
MAKE_LIB_OPTS:=$(MAKEFLAGS)

BUILDROOT := $(CURDIR)
TARGETDIR := $(BUILDROOT)/dist
SRCDIR    := $(BUILDROOT)/src
INCLUDEDIR:=$(BUILDROOT)/include
LIBDIR    := $(BUILDROOT)/lib
BINDIR    := $(BUILDROOT)/bin

export PKG_CONFIG_LIBDIR:=$(LIBDIR)

export DEB_BUILD_GNU_TYPE:=$(shell dpkg-architecture -qDEB_BUILD_GNU_TYPE)
export DEB_HOST_GNU_TYPE:="i586-pc-mingw32"

WGET := wget -nv -T 60 -N --no-proxy

HOST_TOOL_PREFIX:=i586-mingw32msvc

REALCC:=$(HOST_TOOL_PREFIX)-gcc
REALCXX:=$(HOST_TOOL_PREFIX)-g++
CONCURRENCY:=1

MACH_FLAGS:=-march=pentium-mmx
OPTI_FLAGS:=-O3 -fomit-frame-pointer
#	IX86_TYPE according to Intel
#	0 : 8086/88, 2 = 80286, 3 = 80386
#	4 = 80486, 5 = Pentium(R) Processor, 6 = PentiumPro(R) Processor
#       7 or higher = Processor beyond the PentiumPro6(R) Processor
IX86_TYPE:=5


ifeq ($(DEBUG),1)
        export CC=$(REALCC) -mms-bitfields -g 
        export CXX=$(REALCXX) -mms-bitfields -g
else
        export CC=$(REALCC) -mms-bitfields 
        export CXX=$(REALCXX) -mms-bitfields 
endif

export LD:=$(HOST_TOOL_PREFIX)-ld
export NM:=$(HOST_TOOL_PREFIX)-nm
export AR:=$(HOST_TOOL_PREFIX)-ar
export RANLIB:=$(HOST_TOOL_PREFIX)-ranlib
export DLLTOOL:=$(HOST_TOOL_PREFIX)-dlltool
export DLLWRAP:=$(HOST_TOOL_PREFIX)-dllwrap
export OBJDUMP:=$(HOST_TOOL_PREFIX)-objdump
export WINDRES:=$(HOST_TOOL_PREFIX)-windres
export STRIP:=$(HOST_TOOL_PREFIX)-strip
export AS:=$(HOST_TOOL_PREFIX)-as
export CFLAGS:=-I$(INCLUDEDIR) $(MACH_FLAGS) -DPTRACING
export CXXFLAGS:=-I$(INCLUDEDIR) $(MACH_FLAGS) -DPTRACING
export LDFLAGS:=-L$(LIBDIR)  $(MACH_FLAGS)
export PKG_CONFIG_PATH:=$(LIBDIR)/pkgconfig
export PATH:=$(BINDIR):$(PATH)
export LDD:=$(BUILDROOT)/$(HOST_TOOL_PREFIX)-ldd.sh


EKIGA_VER = 3.0.1
EKIGA_REV = HEAD
EKIGA_URL = http://svn.gnome.org/svn/ekiga/branches/gnome-2-24
EKIGA_ARCHIVE := ekiga
EKIGA_DIR = $(BUILDROOT)/ekiga
EKIGA_PREFIX:= $(TARGETDIR)/Ekiga
EKIGA_WIN32_DIR := $(CURDIR)
EKIGA_WIN32_DIFF_DIR := $(EKIGA_WIN32_DIR)/diff
EKIGA_INSTALLER_DIR := $(BUILDROOT)/nsisinstaller
EKIGA_INSTALLER := ekiga-setup-${EKIGA_VER}.exe

OPAL_VER:= 3.4.2
OPAL_REV:= HEAD
OPAL_URL:= https://opalvoip.svn.sourceforge.net/svnroot/opalvoip/opal/branches/v3_4
OPAL_ARCHIVE := opal
OPAL_DIR:= $(BUILDROOT)/opal

PTLIB_VER:= 2.4.2
PTLIB_REV:= HEAD
PTLIB_URL:= https://opalvoip.svn.sourceforge.net/svnroot/opalvoip/ptlib/branches/v2_4
PTLIB_DIR:= $(BUILDROOT)/ptlib
PTLIB_ARCHIVE := ptlib

OPENLDAP_VER := 2.3.28
OPENLDAP_DIR := $(BUILDROOT)/openldap-$(OPENLDAP_VER)
OPENLDAP_ARCHIVE := openldap-$(OPENLDAP_VER).tgz
OPENLDAP_URL := http://www.openldap.org/software/download/OpenLDAP/openldap-release

OGG_VER := 1.1.3
OGG_DIR := $(BUILDROOT)/libogg-$(OGG_VER)
OGG_ARCHIVE := libogg-$(OGG_VER).tar.gz
OGG_URL := http://downloads.xiph.org/releases/ogg

SPEEX_VER := 1.2rc1
SPEEX_DIR := $(BUILDROOT)/speex-$(SPEEX_VER)
SPEEX_ARCHIVE := speex-$(SPEEX_VER).tar.gz
SPEEX_URL := http://downloads.us.xiph.org/releases/speex

GTK_VER := 2.12.11
GTK_URL := http://ftp.gnome.org/pub/gnome/binaries/win32/gtk+/2.12
GTK_ARCHIVE := gtk+-bundle-$(GTK_VER).zip

XML2_VER := 2.6.32
XML2_DIR := libxml2-$(XML2_VER)
XML2_URL := ftp://xmlsoft.org/libxml2
XML2_ARCHIVE := libxml2-$(XML2_VER).tar.gz

EXPAT_VER = 2.0.0
EXPAT_DIR = $(BUILDROOT)/expat-$(EXPAT_VER)
EXPAT_ARCHIVE := expat-$(EXPAT_VER).tar.gz
EXPAT_URL := http://kent.dl.sourceforge.net/sourceforge/expat

REGEX_DIR := $(BUILDROOT)/regex
REGEX_ARCHIVE := regex

SIGC_VER := 2.0.17
SIGC_URL := http://ftp.gnome.org/pub/GNOME/sources/libsigc++/2.0
SIGC_ARCHIVE := libsigc++-$(SIGC_VER).tar.gz
SIGC_DIR := libsigc++-$(SIGC_VER)

GIMP_URL := http://www.gimp.org/~tml/gimp/win32

ICONV_VER := 1.9.1
ICONV_ARCHIVE := libiconv-$(ICONV_VER).bin.woe32.zip

FFMPEG_VER := 14200
FFMPEG_URL := svn://svn.mplayerhq.hu/ffmpeg/trunk
FFMPEG_DIR := $(BUILDROOT)/ffmpeg
FFMPEG_ARCHIVE := ffmpeg

X264_VER := 676
X264_URL := git://git.videolan.org/x264.git
X264_DIR := $(BUILDROOT)/x264
X264_ARCHIVE := x264

THEORA_VER := 1.0RC1
THEORA_DIR := $(BUILDROOT)/libtheora-$(THEORA_VER)
THEORA_ARCHIVE := libtheora-$(THEORA_VER).zip
THEORA_URL := http://downloads.us.xiph.org/releases/theora

NSIS_VER := 2.29
NSIS_URL := http://mesh.dl.sourceforge.net/nsis
NSISPROCESSES_URL := http://nsis.sourceforge.net/mediawiki/images/f/f3/
NSISSYTEM_DIR := /usr/share/nsis/Contrib/Modern\ UI
NSISPLUGIN_DIR := $(BUILDROOT)/nsisplugins

LN:=ln
RM:=rm
CP:=cp
MV:=mv
SVN:=svn
GIT:=git
ACLOCAL:=aclocal-1.9

DOC_LINGUAS=bg de es fr pt_BR sv uk

ifneq (,$(CONCURRENCY))
        MAKEOPTS+=-j$(CONCURRENCY)
endif

confflags:=--prefix=$(BUILDROOT) --build=$(DEB_BUILD_GNU_TYPE) --host=$(DEB_HOST_GNU_TYPE)
confptlib:=--prefix=$(BUILDROOT) --build=$(DEB_BUILD_GNU_TYPE) --host=$(DEB_HOST_GNU_TYPE) \
	   --with-directx-includedir=$(BUILDROOT)/directx \
	   --disable-qos --disable-vidfile --disable-alsa --disable-esd --disable-oss --disable-sunaudio \
	   --disable-v4l --disable-v4l2 --disable-bsdvideo --disable-avc --disable-dc
confopal :=--prefix=$(BUILDROOT) --build=$(DEB_BUILD_GNU_TYPE) --host=$(DEB_HOST_GNU_TYPE) \
	   --with-ffmpeg-dir=$(FFMPEG_DIR) --disable-ffmpeg-h263
confekiga:= $(confflags) \
	    --with-directx-dir=$(BUILDROOT)/directx --prefix=$(EKIGA_PREFIX) \
	    --bindir=$(EKIGA_PREFIX) --datadir=$(EKIGA_PREFIX) \
	    --sysconfdir=$(EKIGA_PREFIX) --libdir=$(EKIGA_PREFIX) --disable-gnome \
	    --disable-dbus --disable-avahi --disable-xv --with-ldap-dir=$(OPENLDAP_DIR) \
	    --enable-static-libs

confffmpeg:=--cross-prefix=i586-mingw32msvc- --enable-shared --enable-memalign-hack \
	    --enable-cross-compile --disable-ffserver --target-os=mingw32 \
	    --enable-w32threads --arch=i686  \
	    --disable-parsers --enable-parser=h264 --enable-parser=mpeg4video --enable-parser=h263 \
	    --disable-decoders --enable-decoder=h264 --enable-decoder=mpeg4  --enable-decoder=h263 \
	    --disable-muxers --enable-muxer=h264 --enable-muxer=mpeg4 --enable-muxer=h263 \
	    --disable-encoders --enable-encoder=mpeg4 --enable-encoder=h263 \
	    --disable-protocols --disable-bsfs --disable-ffplay --prefix=$(BUILDROOT) \
            --libdir=$(LIBDIR) --extra-ldflags=-Wl,-static
#	    --disable-network

confx264:=  --disable-avis-input --prefix=$(BUILDROOT) --host=$(DEB_HOST_GNU_TYPE)

conftheora:=--prefix=$(BUILDROOT) --build=$(DEB_BUILD_GNU_TYPE) --host=$(DEB_HOST_GNU_TYPE) \
            --disable-vorbistest --disable-sdltest --disable-examples

all: binaries $(TARGETDIR)/$(EKIGA_INSTALLER)
	@echo "Built on $(shell date)"

update-sources::
	@echo "Updating on $(shell date)"

update: update-sources

binaries:
	echo Checking prerequisites...
	hash $(REALCC) $(REALCXX) $(LD) $(NM) $(AR) $(RANLIB) $(DLLTOOL) $(DLLWRAP) $(OBJDUMP) $(WINDRES) $(AS)||(echo You need to install mingw32;exit 1)
	hash tar unzip zip wget $(ACLOCAL) autoheader libtoolize automake-1.9 autoconf pkg-config gnome-autogen.sh flex yasm bison gdk-pixbuf-csource||(echo we need more binaries ;exit 1)
	[ -f /usr/share/gnome-doc-utils/gnome-doc-utils.make ]||(echo gnome-doc-tools not installed;exit 1)
	[ -f /usr/lib/pkgconfig/gconf-2.0.pc ]||(echo libgconf2-dev not installed;exit 1)
	[ -f /usr/lib/pkgconfig/avahi-glib.pc ]||(echo libavahi-glib-dev not installed;exit 1)
	[ -f /usr/lib/pkgconfig/avahi-client.pc ]||(echo libavahi-client-dev not installed;exit 1)
	hash makensis||(echo nsis not installed;exit1) 	 
	hash xmlto||(echo xmlto not installed;exit1)
#       [ $(BUILDROOT) = $(CURDIR) ]||(echo You are supposed to run this as Makefile in $(BUILDROOT);exit 1)
	mkdir -p $(INCLUDEDIR) $(LIBDIR)/pkgconfig $(SRCDIR) $(BINDIR)
	$(RM) -f $(BINDIR)/cygpath
	ln -s /bin/true $(BINDIR)/cygpath
	touch $@

### REGEX
update-sources:: 
	echo "--- Getting libregex..."
	mkdir -p $(SRCDIR)/$(REGEX_ARCHIVE)
	cd $(SRCDIR)/$(REGEX_ARCHIVE); \
	for i in regex.c regexec.c regex.h regex_internal.c regex_internal.h regcomp.c alloca_.h alloca.c strcase.h localcharset.c localcharset.h;do \
	  $(WGET) http://cvs.savannah.nongnu.org/viewcvs/*checkout*/gnulib/gnulib/lib/$$i ;\
	done

$(REGEX_DIR)/regex.h: $(SRCDIR)/$(REGEX_ARCHIVE)
	$(RM) -rf $(REGEX_DIR) 
	mkdir -p $(REGEX_DIR)
	$(CP) $(SRCDIR)/$(REGEX_ARCHIVE)/* $(REGEX_DIR)
	$(MV) $(REGEX_DIR)/alloca_.h $(REGEX_DIR)/alloca.h
	echo '//' >> $(REGEX_DIR)/config.h
	touch $@

$(LIBDIR)/libregex.a: binaries $(REGEX_DIR)/regex.h
	cd $(REGEX_DIR) ;\
	$(CC) -I$(INCLUDEDIR) -I. -c regex.c ;\
	$(CC) -I$(INCLUDEDIR) -I. -c alloca.c ;\
	$(CC) -I$(INCLUDEDIR) -I. -DLIBDIR=$(INCLUDEDIR) -c localcharset.c ;\
	$(AR) r libregex.a regex.o alloca.o localcharset.o
	$(CP) $(REGEX_DIR)/libregex.a $(LIBDIR)
	$(CP) $(REGEX_DIR)/regex.h $(INCLUDEDIR)
	$(CP) $(REGEX_DIR)/alloca.h $(INCLUDEDIR)
	$(CP) $(REGEX_DIR)/localcharset.h $(INCLUDEDIR)
	touch $@

### libsigc++
update-sources::
	echo --- Getting libsigc++ ...
	$(WGET) -P src $(SIGC_URL)/$(SIGC_ARCHIVE)

$(SIGC_DIR)/config.status: $(SRCDIR)/$(SIGC_ARCHIVE) 
	tar xfz $(SRCDIR)/$(SIGC_ARCHIVE)
	(cd $(SIGC_DIR); ./configure $(confflags) --enable-shared)
	touch $@

$(LIBDIR)/libsigc++.a:  $(SIGC_DIR)/config.status
	$(MAKE) $(MAKEOPTS) -C $(SIGC_DIR)
	$(MAKE) $(MAKEOPTS) -C $(SIGC_DIR) install
	touch $@

### expat
update-sources::
	echo --- Getting EXPAT ...
	$(WGET) -P src $(EXPAT_URL)/$(EXPAT_ARCHIVE)

$(EXPAT_DIR)/config.status: $(SRCDIR)/$(EXPAT_ARCHIVE) 
	tar xfz $(SRCDIR)/$(EXPAT_ARCHIVE)
	(cd $(EXPAT_DIR); ./configure $(confflags))
	touch $@

$(INCLUDEDIR)/expat.h:
	install -m 644 $(EXPAT_DIR)/lib/expat.h $(INCLUDEDIR)/
	touch $@

$(LIBDIR)/libexpat.a:  $(EXPAT_DIR)/config.status $(INCLUDEDIR)/expat.h
	$(MAKE) $(MAKEOPTS) -C $(EXPAT_DIR)
	$(MAKE) $(MAKEOPTS) -C $(EXPAT_DIR) install
	touch $@

### OPENLDAP
update-sources:: 
	echo --- Getting OpenLDAP...
	$(WGET) -P $(SRCDIR) $(OPENLDAP_URL)/$(OPENLDAP_ARCHIVE)

$(OPENLDAP_DIR)/configure: $(LIBDIR)/libregex.a $(SRCDIR)/$(OPENLDAP_ARCHIVE)
	rm -f $(LIBDIR)/libldap.dll
	tar xfz $(SRCDIR)/$(OPENLDAP_ARCHIVE) -C $(BUILDROOT)
	sed -i -e 's#windres#$(WINDRES)#g' $(OPENLDAP_DIR)/libraries/liblutil/Makefile.in
	touch $@

$(OPENLDAP_DIR)/config.status: binaries $(OPENLDAP_DIR)/configure
	#-$(MAKE) -C $(OPENLDAP_DIR) clean
	rm -f $(LIBDIR)/libldap.dll
	ln -sf $(INCLUDEDIR)/regex.h $(OPENLDAP_DIR)/include/
	cd $(OPENLDAP_DIR);./configure --with-cyrus-sasl=no --enable-bdb=no --enable-hdb=no $(confflags)
	$(MAKE) $(MAKEOPTS) -C $(OPENLDAP_DIR) depend
	touch $@

$(LIBDIR)/libldap.dll: binaries $(OPENLDAP_DIR)/config.status
	$(MAKE) $(MAKE_LIB_OPTS) -C $(OPENLDAP_DIR)/libraries/liblutil
	$(CP) -f $(OPENLDAP_DIR)/libraries/liblutil/liblutil.a $(LIBDIR)
	$(MAKE) $(MAKE_LIB_OPTS) -C $(OPENLDAP_DIR)/libraries/liblber all install 
	$(MAKE) $(MAKE_LIB_OPTS) -C $(OPENLDAP_DIR)/include install
	$(MAKE) $(MAKE_LIB_OPTS) -C $(OPENLDAP_DIR)/libraries/libldap MOD_LIBS="-llutil" all install
	touch $@

### PTLib
update-sources:: 
	echo --- Getting PTLib $(PTLIB_REV) ...
	$(SVN) co $(PTLIB_URL) $(SRCDIR)/$(PTLIB_ARCHIVE) -r $(PTLIB_REV)

$(PTLIB_DIR)/configure: binaries $(LIBDIR)/libldap.dll $(LIBDIR)/libogg.a $(LIBDIR)/libspeex.a $(SRCDIR)/$(PTLIB_ARCHIVE)  $(LIBDIR)/libexpat.a
	rm -f $(LIBDIR)/libpt_s.a
	rm -rf $(PTLIB_DIR)
	$(CP) -r $(SRCDIR)/$(PTLIB_ARCHIVE) $(PTLIB_DIR)	
	touch $@

$(PTLIB_DIR)/config.status: binaries $(PTLIB_DIR)/configure
	rm -f $(LIBDIR)/libpt_s.a
	(cd $(PTLIB_DIR)/;./configure $(confptlib) )
	touch $@

$(LIBDIR)/libpt_s.a: binaries $(PTLIB_DIR)/config.status
ifeq ($(DEBUG),1)
	$(MAKE) $(MAKEOPTS) DEBUG=1 debug -C $(PTLIB_DIR)
	$(MAKE) DEBUG=1 -C $(PTLIB_DIR) install
else
	$(MAKE) $(MAKEOPTS) -C $(PTLIB_DIR)
	$(MAKE) -C $(PTLIB_DIR) install
endif
	touch $@

### libogg
update-sources::
	echo --- Getting Ogg ...
	$(WGET) -P $(SRCDIR) $(OGG_URL)/$(OGG_ARCHIVE)

$(OGG_DIR)/config.status: binaries $(SRCDIR)/$(OGG_ARCHIVE)
	rm -f $(LIBDIR)/libogg.a
	tar xfz $(SRCDIR)/$(OGG_ARCHIVE) -C $(BUILDROOT)
	(cd $(OGG_DIR)/;./configure $(confflags) \
	)

$(LIBDIR)/libogg.a: binaries $(OGG_DIR)/config.status
	rm -f $(LIBDIR)/libogg.a
	$(MAKE) $(MAKEOPTS) -C $(OGG_DIR)
	$(MAKE) -C $(OGG_DIR) install

### libspeex
update-sources:: 
	echo --- Getting Vorbis ...
	$(WGET) -P $(SRCDIR) $(SPEEX_URL)/$(SPEEX_ARCHIVE)

$(SPEEX_DIR)/config.status: binaries $(SRCDIR)/$(SPEEX_ARCHIVE)
	rm -f $(LIBDIR)/libspeex.a
	tar xfz $(SRCDIR)/$(SPEEX_ARCHIVE) -C $(BUILDROOT)
	(cd $(SPEEX_DIR)/;LDFLAGS="$$LDFLAGS -no-undefined" ./configure $(confflags) \
	)

$(LIBDIR)/libspeex.a: binaries $(SPEEX_DIR)/config.status
	rm -f $(LIBDIR)/libspeex.a
	$(MAKE) $(MAKE_LIB_OPTS) -C $(SPEEX_DIR)
	$(MAKE) -C $(SPEEX_DIR) install

### ffmpeg
update-sources::
	echo --- Getting FFMPEG ---
	$(SVN) co $(FFMPEG_URL) $(SRCDIR)/$(FFMPEG_ARCHIVE) -r $(FFMPEG_VER)

$(FFMPEG_DIR)/config.status: $(SRCDIR)/$(FFMPEG_ARCHIVE)
	rm -rf $(FFMPEG_DIR)
	$(CP) -r $(SRCDIR)/$(FFMPEG_ARCHIVE) $(FFMPEG_DIR)
	(cd $(FFMPEG_DIR)/; \
	./configure $(confffmpeg) \
	)
	touch $@

$(BINDIR)/avcodec.dll: $(FFMPEG_DIR)/config.status
	$(MAKE) $(MAKE_LIB_OPTS) -C $(FFMPEG_DIR)/libavutil
	$(MAKE) $(MAKE_LIB_OPTS) -C $(FFMPEG_DIR)/libavcodec
	$(MAKE) $(MAKE_LIB_OPTS) -C $(FFMPEG_DIR)/libavformat
	$(MAKE) $(MAKE_LIB_OPTS) -C $(FFMPEG_DIR)/libavdevice
	$(MAKE) $(MAKE_LIB_OPTS) -C $(FFMPEG_DIR)
	$(MAKE) -C $(FFMPEG_DIR) install
	touch $@

### x264
update-sources:: 
	echo --- Getting x264 ---
	rm -rf $(SRCDIR)/$(X264_ARCHIVE)
	$(GIT) clone $(X264_URL) $(SRCDIR)/$(X264_ARCHIVE)
	echo "Updated to revision $(X264_VER)"
	touch $(SRCDIR)/$(X264_ARCHIVE)

$(X264_DIR)/config.status: $(SRCDIR)/$(X264_ARCHIVE)
	rm -rf $(X264_DIR)
	$(CP) -r $(SRCDIR)/$(X264_ARCHIVE) $(X264_DIR)
	(cd $(X264_DIR)/; \
	CC=$(REALCC) ./configure $(confx264); \
	sed -i -e 's#ranlib#$(RANLIB)#g' $(X264_DIR)/Makefile; \
	sed -i -e 's#strip#$(STRIP)#g' $(X264_DIR)/Makefile; \
	sed -i -e 's#ar #$(AR) #g' $(X264_DIR)/Makefile )
	touch $@

$(LIBDIR)/libx264.a: $(X264_DIR)/config.status
	$(MAKE) $(MAKE_LIB_OPTS) -C $(X264_DIR)
	$(MAKE) -C $(X264_DIR) install
	touch $@

### theora
update-sources:: 
	echo --- Getting theoras ...
	$(WGET) -P $(SRCDIR) $(THEORA_URL)/$(THEORA_ARCHIVE)

$(THEORA_DIR)/config.status: binaries $(SRCDIR)/$(THEORA_ARCHIVE)
	rm -f $(THEORA_DIR)
	unzip -u $(SRCDIR)/$(THEORA_ARCHIVE) -d $(BUILDROOT)
	(cd $(THEORA_DIR)/;./configure $(conftheora) \
	)
	touch $@

$(LIBDIR)/libtheora.a: $(THEORA_DIR)/config.status
	$(MAKE) $(MAKE_LIB_OPTS) -C $(THEORA_DIR)
	$(MAKE) -C $(THEORA_DIR) install
	sed -i -e "s#library_names=''#library_names='libtheora\.a'#g" $(LIBDIR)/libtheora.la
	touch $@

### OPAL
update-sources::
	echo --- Getting OPAL $(OPAL_REV)...
	$(SVN) co $(OPAL_URL) $(SRCDIR)/$(OPAL_ARCHIVE) -r $(OPAL_REV)

$(OPAL_DIR)/configure: binaries 
	rm -rf $(OPAL_DIR) $(LIBDIR)/libopal_s.a
	$(CP) -r $(SRCDIR)/$(OPAL_ARCHIVE) $(OPAL_DIR)
	touch $@

$(OPAL_DIR)/config.status: binaries $(OPAL_DIR)/configure $(LIBDIR)/libogg.a $(LIBDIR)/libspeex.a $(LIBDIR)/libpt_s.a $(SRCDIR)/$(OPAL_ARCHIVE) $(BINDIR)/avcodec.dll $(LIBDIR)/libx264.a $(LIBDIR)/libtheora.a
	rm -f $(LIBDIR)/libopal_s.a
ifeq ($(DEBUG),1)
	(cd $(OPAL_DIR)/; ./configure $(confopal) --enable-debug )
else
	(cd $(OPAL_DIR)/; ./configure $(confopal)  )
endif
	touch $@

$(LIBDIR)/libopal_s.a: binaries $(OPAL_DIR)/config.status
	rm -f $(LIBDIR)/libopal_s.a
	$(MAKE) $(MAKEOPTS) -C $(OPAL_DIR) 
	$(MAKE) -C $(OPAL_DIR) install 
	touch $@

### libgtk
update-sources::
	echo --- Getting GTK2+ ...
	$(WGET) -P $(SRCDIR) $(GTK_URL)/$(GTK_ARCHIVE)

$(LIBDIR)/pkgconfig/gtk+-2.0.pc: $(SRCDIR)/$(GTK_ARCHIVE)
	unzip -u $(SRCDIR)/$(GTK_ARCHIVE) -d $(BUILDROOT)
	sed -i -e 's#^prefix=.*$$#prefix=$(BUILDROOT)#g' $(LIBDIR)/pkgconfig/g*.pc $(BINDIR)/glib-gettextize
	sed -i -e 's#^prefix=.*$$#prefix=$(BUILDROOT)#g' $(LIBDIR)/pkgconfig/atk*.pc
	sed -i -e 's#^prefix=.*$$#prefix=$(BUILDROOT)#g' $(LIBDIR)/pkgconfig/libpng*.pc
	sed -i -e 's#^prefix=.*$$#prefix=$(BUILDROOT)#g' $(LIBDIR)/pkgconfig/pango*.pc
	sed -i -e 's#^prefix=.*$$#prefix=$(BUILDROOT)#g' $(LIBDIR)/pkgconfig/cairo*.pc

	touch $@

### libiconv
update-sources:: 
	echo --- Getting Iconv ...
	$(WGET) -P $(SRCDIR) $(GIMP_URL)/$(ICONV_ARCHIVE)

$(LIBDIR)/libiconv.a: $(SRCDIR)/$(ICONV_ARCHIVE)
	unzip -u $(SRCDIR)/$(ICONV_ARCHIVE) -d $(BUILDROOT)
	$(RM) -f README.libiconv
	touch $@

### libxml2
update-sources::
	echo --- Getting libXML2 ...
	$(WGET) -P $(SRCDIR) $(XML2_URL)/$(XML2_ARCHIVE)

$(XML2_DIR)/config.status: binaries $(SRCDIR)/$(XML2_ARCHIVE)
	rm -f $(LIBDIR)/libxml2.a
	tar xfz $(SRCDIR)/$(XML2_ARCHIVE) -C $(BUILDROOT)
	(cd $(XML2_DIR)/;./configure $(confflags) --without-python \
	)

$(LIBDIR)/libxml2.a: binaries $(XML2_DIR)/config.status
	rm -f $(LIBDIR)/libxml2.a
	$(MAKE) $(MAKEOPTS) -C $(XML2_DIR)
	$(MAKE) $(MAKEOPTS) -C $(XML2_DIR) install

### GnomeMeeting / Ekiga
update-sources::
	echo --- Getting Ekiga $(EKIGA_REV)...
	$(SVN) co $(EKIGA_URL) $(SRCDIR)/$(EKIGA_ARCHIVE) -r $(EKIGA_REV)

$(EKIGA_DIR)/autogen.sh: binaries $(SRCDIR)/$(EKIGA_ARCHIVE)
	rm -rf $(EKIGA_DIR)
	$(CP) -r $(SRCDIR)/$(EKIGA_ARCHIVE) $(EKIGA_DIR)
	touch $@

$(EKIGA_DIR)/config.status: binaries $(EKIGA_DIR)/autogen.sh $(LIBDIR)/libpt_s.a $(LIBDIR)/libopal_s.a \
	$(LIBDIR)/pkgconfig/gtk+-2.0.pc $(LIBDIR)/libxml2.a $(LIBDIR)/libiconv.a $(LIBDIR)/libsigc++.a
	rm -f $(EKIGA_DIR)/src/ekiga.exe
ifeq ($(DEBUG),1)
	(cd $(EKIGA_DIR)/; ACLOCAL_FLAGS="-I $(BUILDROOT)/share/aclocal"; ./autogen.sh $(confekiga) --enable-debug )
else
	(cd $(EKIGA_DIR)/; ACLOCAL_FLAGS="-I $(BUILDROOT)/share/aclocal"; ./autogen.sh $(confekiga) )
endif
	touch $@

$(EKIGA_DIR)/src/ekiga.exe: binaries $(EKIGA_DIR)/config.status
	$(WINDRES) $(EKIGA_DIR)/ekiga.rc $(EKIGA_DIR)/ekiga_rc.o -I $(INCLUDEDIR)
	$(MAKE) $(MAKEOPTS) -C $(EKIGA_DIR)
	$(MAKE) $(MAKEOPTS) -C $(EKIGA_DIR)/help
	touch $@

$(TARGETDIR)/zips: binaries $(EKIGA_DIR)/src/ekiga.exe
	-$(RM) -rf $(TARGETDIR)
	make -C ekiga install
	mkdir -p $(EKIGA_PREFIX)/plugins
	
	for plugin in $(LIBDIR)/opal*/codecs/audio/*; do \
	        install -m 755 $$plugin $(EKIGA_PREFIX)/plugins ;\
	done
	for plugin in $(LIBDIR)/opal*/codecs/video/*; do \
	        install -m 755 $$plugin $(EKIGA_PREFIX)/plugins ;\
	done
	for lib in libexpat-1.dll liblber.dll libldap.dll libxml2-2.dll libogg-0.dll libspeex-1.dll \
	           libspeexdsp-1.dll libtheora-0.dll avcodec.dll; do \
	        install -m 755 $(BINDIR)/$$lib $(EKIGA_PREFIX) ;\
	done
	install -m 755  $(SIGC_DIR)/sigc++/.libs/libsigc-2.0-0.dll $(EKIGA_PREFIX)

ifeq ($(DEBUG),0) 	 
	$(STRIP) $(EKIGA_PREFIX)/*.dll
	$(STRIP) $(EKIGA_PREFIX)/*.exe; 
endif

	unzip -u -j $(SRCDIR)/$(GTK_ARCHIVE) "bin/*.dll" -d $(EKIGA_PREFIX) ;\
	unzip -u $(SRCDIR)/$(GTK_ARCHIVE) "etc/*" -d $(EKIGA_PREFIX) ;\
	unzip -u $(SRCDIR)/$(GTK_ARCHIVE) "lib/*.dll" -d $(EKIGA_PREFIX) ;\
	unzip -u $(SRCDIR)/$(GTK_ARCHIVE) "share/glib*" -d $(EKIGA_PREFIX) ; \
	unzip -u $(SRCDIR)/$(GTK_ARCHIVE) "share/locale/*" -d $(EKIGA_PREFIX) ; \
	unzip -u $(SRCDIR)/$(GTK_ARCHIVE) "share/themes/*" -d $(EKIGA_PREFIX) ; \

	for lc in C $(DOC_LINGUAS); do \
		mkdir -p $(EKIGA_PREFIX)/help/$$lc; \
		if test -f $(EKIGA_DIR)/help/$$lc/ekiga.xml; then \
			xmlto -o $(EKIGA_PREFIX)/help/$$lc/ --skip-validation html $(EKIGA_DIR)/help/$$lc/ekiga.xml; \
		fi ;\
		install -m 644 $(EKIGA_DIR)/help/$$lc/ekiga.xml $(EKIGA_PREFIX)/help/$$lc/;\
		if test -d $(EKIGA_DIR)/help/$$lc/figures; then \
			mkdir -p $(EKIGA_PREFIX)/help/$$lc/figures; \
			install -m 644 $(EKIGA_DIR)/help/$$lc/figures/*.png $(EKIGA_PREFIX)/help/$$lc/figures; \
		fi \
	done
	(cd $(TARGETDIR)/;zip -9r ekiga.zip Ekiga )
	touch $@

#### README about NSIS installer ####
#
# NSIS package shipped with linux distributions does not includes some of the plugins needed.
# - System.dll : You need to get it from the windows zip archive (http://sourceforge.net/projects/nsis) and extract 
#               /Plugins/System.dll TO /usr/share/nsis/Plugins/
#		/Contrib/Modern UI/System.nsh TO /usr/share/nsis/Contrib/System/
# - Process.dll : Is a plugin that can be found here http://nsis.sourceforge.net/Processes_plug-in
#   		extract /bin/Processes.dll TO /usr/share/nsis/Plugins/
#   		
###########################

update-sources::
	echo --- Getting NSIS Installer ...
	$(WGET) -P $(SRCDIR) $(NSIS_URL)/nsis-$(NSIS_VER).zip

$(NSISPLUGIN_DIR)/SystemLocal.dll: $(SRCDIR)/nsis-$(NSIS_VER).zip
	mkdir -p $(NSISPLUGIN_DIR)
	unzip -u -j $(SRCDIR)/nsis-$(NSIS_VER).zip nsis-$(NSIS_VER)/Plugins/System.dll -d $(NSISPLUGIN_DIR)
	$(MV) $(NSISPLUGIN_DIR)/System.dll $(NSISPLUGIN_DIR)/SystemLocal.dll
	touch $@

update-sources::
	echo --- Getting NSIS Processes Plugin ...
	$(WGET) -P $(SRCDIR) $(NSISPROCESSES_URL)/Processes.zip

$(NSISPLUGIN_DIR)/Processes.dll: $(SRCDIR)/Processes.zip
	mkdir -p $(NSISPLUGIN_DIR)
	unzip -u -j $(SRCDIR)/Processes.zip setup-processes/bin/Processes.dll -d $(NSISPLUGIN_DIR)
	touch $@

$(TARGETDIR)/$(EKIGA_INSTALLER): $(TARGETDIR)/zips  $(NSISPLUGIN_DIR)/SystemLocal.dll $(NSISPLUGIN_DIR)/Processes.dll
	(makensis -DEKIGA_VERSION=$(EKIGA_VER) -DEKIGA_DIR=$(EKIGA_DIR) -DBUILDROOT=$(BUILDROOT) \
	 -DINSTALLER_DIR=$(EKIGA_INSTALLER_DIR) -DLIB_DIR=$(LIBDIR) -DTARGET_DIR=$(TARGETDIR) \
	 -DCROSS_COMPILING=true -DGTK_VERSION=undefined\
	 -DNSISSYSTEMDIR=$(NSISSYTEM_DIR) -DNSISPLUGINDIR=$(NSISPLUGIN_DIR) \
	 $(EKIGA_INSTALLER_DIR)/ekiga.nsi )
	touch $@

clean:
	-$(RM) -rf $(TARGETDIR)

distclean:
	-$(RM) -rf $(TARGETDIR)
	-$(RM) -rf $(OPENLDAP_DIR)
	-$(RM) -rf $(BUILDROOT)/share/
	-$(RM) -rf $(BINDIR)
	-$(RM) -rf $(BUILDROOT)/contrib
	-$(RM) -rf $(BUILDROOT)/etc
	-$(RM) -rf $(INCLUDEDIR)
	-$(RM) -rf $(LIBDIR)
	-$(RM) -rf $(OGG_DIR)
	-$(RM) -rf $(XML2_DIR)
	-$(RM) -rf $(BUILDROOT)/man
	-$(RM) -rf $(BUILDROOT)/manifest
	-$(RM) -rf $(OPAL_DIR)
	-$(RM) -rf $(EKIGA_DIR)
	-$(RM) -rf $(PTLIB_DIR)
	-$(RM) -rf $(REGEX_DIR)
	-$(RM) -rf $(SIGC_DIR)
	-$(RM) -rf $(SPEEX_DIR)
	-$(RM) -rf $(EXPAT_DIR)
	-$(RM) -rf $(FFMPEG_DIR)
	-$(RM) -rf $(X264_DIR)
	-$(RM) -rf $(THEORA_DIR)
	-$(RM) -rf make
	-$(RM) -f binaries

srclean:
	-$(RM) -rf $(SRCDIR)/*

.PHONY: clean distclean update update-sources
