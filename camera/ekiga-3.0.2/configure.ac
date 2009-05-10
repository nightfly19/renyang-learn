AC_INIT(ekiga, 3.0.2, http://bugzilla.gnome.org/enter_bug.cgi?product=ekiga)
AC_CANONICAL_TARGET
AC_PREREQ(2.53)
AC_CONFIG_SRCDIR(src/gui/main.cpp)
AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)

AM_MAINTAINER_MODE
AM_CONFIG_HEADER(config.h)



dnl ###########################################################################
dnl   Definitions
dnl ###########################################################################

MAJOR_VERSION=3
MINOR_VERSION=0
BUILD_TYPE=ReleaseCode
BUILD_NUMBER=2

SCHEMA_AGE=62

PTLIB_REC_VERSION="2.4.4"
OPAL_REC_VERSION="3.4.4"

AC_DEFINE_UNQUOTED(MAJOR_VERSION, $MAJOR_VERSION,[fix])
AC_DEFINE_UNQUOTED(MINOR_VERSION, $MINOR_VERSION,[fix])
AC_DEFINE_UNQUOTED(BUILD_TYPE, $BUILD_TYPE,[fix])
AC_DEFINE_UNQUOTED(BUILD_NUMBER, $BUILD_NUMBER,[fix])

AC_SUBST(SCHEMA_AGE)


dnl ###########################################################################
dnl   Disable / Enable Main Ekiga Components
dnl ###########################################################################

PKG_PROG_PKG_CONFIG

AM_PROG_LIBTOOL

AC_PROG_CC_C99
AC_PROG_CXX
AC_ISC_POSIX
AC_HEADER_STDC

AC_CHECK_DECLS(strcasecmp)

dnl Enable compiler warnings
if test ".$ac_cv_c_compiler_gnu" = .yes; then
   CFLAGS="$CFLAGS -Wall -Wextra -Winit-self -Wswitch-default -Wswitch-enum -Wstrict-aliasing=2 -Wfloat-equal -Wshadow"
   CXXFLAGS="$CXXFLAGS -Wall -Wextra -Winit-self -Wswitch-default -Wswitch-enum -Wstrict-aliasing=2 -Wfloat-equal -Wshadow"
fi

dnl Check the library path
case $host in
  
  *-*-linux*)
    # Test if the compiler is 64bit
    echo 'int i;' > conftest.$ac_ext
    ekiga_cv_cc_64bit_output=no

    if AC_TRY_EVAL(ac_compile); then
      case `/usr/bin/file conftest.$ac_objext` in
        *"ELF 64"*)
          ekiga_cv_cc_64bit_output=yes      
          ;;
      esac
    fi
    rm -rf conftest*  
  ;;
esac

case $host_cpu:$ekiga_cv_cc_64bit_output in

  powerpc64:yes | s390x:yes | sparc64:yes | x86_64:yes)
    libname="lib64"  
    ;;

  *:*)  
    libname="lib"  
    ;;
esac

dnl Check the OS type (more types can be added) 
AC_MSG_CHECKING(whether OS type is supported)
win32=0

case $target_os in

  solaris*)
    gm_platform="solaris"
    ;;

  linux-gnulp | linux-gnu | linux-gnueabi | linux | Linux)
    gm_platform="linux"
    ;;

  FreeBSD* | freebsd* | NetBSD* | netbsd* )
    gm_platform="freebsd"
    ;;

  darwin* )
    gm_platform="macosx"
    ;;

  mingw* )
    gm_platform="mingw"
    CFLAGS="$CFLAGS -DSTATIC_LIBS_USED"
    CXXFLAGS="$CXXFLAGS -DSTATIC_LIBS_USED"
    win32=1
    ;;

  *)
    AC_MSG_ERROR([Only Linux, FreeBSD, NetBSD, Solaris and Mac OS X are supported. Please contact the author to know how you can add more OS types.])
    ;;
esac
AC_MSG_RESULT([yes])

AM_CONDITIONAL(WIN32, test "x${win32}" = "x1")

dnl This ARCH stuff is here for reference only and should be removed in the future...
case $target_os in

  solaris*)
    ARCH_PTLIB_CFLAGS="-DPTRACING -DNDEBUG -DSOLARIS -g "
    ARCH_OPAL_LIBS="-lpt -lopal -ldl -lpthread"
    ;;

  linux-gnulp | linux-gnu | linux-gnueabi | linux | Linux)
    ARCH_PTLIB_CFLAGS="-DPTRACING -DNDEBUG -Os -g -fno-exceptions -felide-constructors"
    ARCH_OPAL_LIBS="-lpt -lopal -ldl -lpthread"
    ;;

  FreeBSD* | freebsd* | NetBSD* | netbsd* )
    ARCH_PTLIB_CFLAGS="-DPTRACING -DNDEBUG -Os -g"
    ARCH_OPAL_LIBS="-lpt -lopal -lpthread"
    ;;

  darwin* )
    ARCH_PTLIB_CFLAGS="-Os -DPTRACING -DNDEBUG -DNO_LONG_DOUBLE -DSTATIC_LIBS_USED"
    ARCH_OPAL_LIBS="-multiply_defined suppress -lh323_Darwin_ppc_r_s -lpt_Darwin_ppc_r_s -lssl -lcrypto -lX11 -framework AudioToolbox -framework CoreAudio -framework CoreServices"
    ;;

  mingw* )
    ARCH_PTLIB_CFLAGS="-mms-bitfields -DPTRACING -DSTATIC_LIBS_USED"
    ARCH_OPAL_LIBS="-mwindows -enable-auto-import -enable-runtime-pseudo-reloc -enable-stdcall-fixup -lpt -lopal -lwinmm -lwsock32 -lsnmpapi -lmpr -lcomdlg32"
    ;;
esac

dnl Check the byte order 
AC_MSG_CHECKING(Byte order)
byte_order="unknown"
AC_ARG_WITH([endianess], 
            [  --with-endianess=big|little Endianess of target system],
	    [byte_order="$withval endian"], 
            [AC_C_BIGENDIAN([byte_order="big endian"], [byte_order="little endian"], [])])

if test "x$byte_order" = "xbig endian"; then
    AC_DEFINE(WORDS_BIGENDIAN,1,[Big Endian Architecture])
else 
    if test "x$byte_order" = "xlittle endian"; then
	AC_DEFINE(WORDS_LITTLEENDIAN,1,[Little Endian Architecture])
    else
      AC_MSG_ERROR(Unable to detect endianess of target system - please specify)
    fi
fi

AC_MSG_RESULT([$byte_order])

dnl ###############################
dnl   Mandatory intltool support
dnl ###############################
IT_PROG_INTLTOOL([0.35.0])


dnl ###############################
dnl   Mandatory GTK support
dnl ###############################
PKG_CHECK_MODULES([GTK], [gtk+-2.0 >= 2.12.0])
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)


dnl ###############################
dnl   Mandatory GLIB support
dnl ###############################
PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.8.0 gmodule-2.0 gobject-2.0 gthread-2.0])
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)


dnl ###############################
dnl   Mandatory SIGC++ support
dnl ###############################
PKG_CHECK_MODULES([SIGC], [sigc++-2.0])
AC_SUBST(SIGC_CFLAGS)
AC_SUBST(SIGC_LIBS)


dnl ###############################
dnl   Mandatory libXML support
dnl ###############################
PKG_CHECK_MODULES([XML], [libxml-2.0])
AC_SUBST(XML_CFLAGS)
AC_SUBST(XML_LIBS)


dnl ###############################
dnl   GNOME support
dnl ###############################
GNOME="disabled"

if test "x${gm_platform}" != "xmingw" ; then
  AC_ARG_ENABLE(gnome, [  --disable-gnome		Disable GNOME support],,enable_gnome=yes)
  
  if test "x$enable_gnome" = "xyes"; then
    PKG_CHECK_MODULES([GNOME], [libgnome-2.0 >= 2.14.0 libgnomeui-2.0 >= 2.14.0], [found_gnome=yes])
    AC_SUBST(GNOME_CFLAGS)
    AC_SUBST(GNOME_LIBS)
    GNOME="enabled"
    AC_DEFINE(HAVE_GNOME,1,[GNOME support])
  fi
fi
AM_CONDITIONAL(HAVE_GNOME, test "x$found_gnome" = "xyes")


dnl ###############################
dnl   GConf support
dnl ###############################
GCONF="disabled"

if test "x${gm_platform}" != "xmingw" ; then
  AC_ARG_ENABLE(gconf, [  --disable-gconf		Disable GConf support],,enable_gconf=yes)
  
  if test "x$enable_gconf" = "xyes"; then
    PKG_CHECK_MODULES([GCONF], [gconf-2.0 >= 2.6.0], [found_gconf=yes])
    AM_GCONF_SOURCE_2
    AC_SUBST(GCONF_CFLAGS)
    AC_SUBST(GCONF_LIBS)
    GCONF="enabled"
    AC_DEFINE(HAVE_GCONF,1,[GConf support])
  else
    AM_CONDITIONAL(GCONF_SCHEMAS_INSTALL, false)
  fi
else
  AM_CONDITIONAL(GCONF_SCHEMAS_INSTALL, false)
fi

AM_CONDITIONAL(HAVE_GCONF, test "x$found_gconf" = "xyes")


dnl ###############################
dnl   Evolution-data-server support
dnl ###############################
EDS="disabled"

if test "x${gm_platform}" != "xmingw" ; then
  AC_ARG_ENABLE(eds, [  --disable-eds			Disable evolution-data-server addressbook support],,enable_eds=yes)
  
  if test "x$enable_eds" = "xyes"; then
    PKG_CHECK_MODULES([EDS], [libebook-1.2], [found_eds=yes])
    AC_SUBST(EDS_CFLAGS)
    AC_SUBST(EDS_LIBS)
    EDS="enabled"
    AC_DEFINE(HAVE_EDS,1,[EDS support])
  fi
fi

AM_CONDITIONAL(HAVE_EDS, test "x$found_eds" = "xyes")


dnl ###############################
dnl   NOTIFY support
dnl ###############################
NOTIFY="disabled"

if test "x${gm_platform}" != "xmingw" ; then
  AC_ARG_ENABLE(notify, [  --disable-notify			Disable libnotify support],,enable_notify=yes)
  
  if test "x$enable_notify" = "xyes"; then
    PKG_CHECK_MODULES([NOTIFY], [libnotify], [found_notify=yes], foo=bar)
    if test "x$found_notify" = "xyes"; then
      AC_SUBST(NOTIFY_CFLAGS)
      AC_SUBST(NOTIFY_LIBS)
      NOTIFY="enabled"
      AC_DEFINE(HAVE_NOTIFY,1,[NOTIFY support])
    fi
  fi
fi

AM_CONDITIONAL(HAVE_NOTIFY, test "x$found_notify" = "xyes")


dnl ###############################
dnl   LDAP support
dnl ###############################
LDAP="disabled"

AC_ARG_ENABLE(ldap, [  --disable-ldap			Disable LDAP addressbook support],,enable_ldap=yes)

if test "x$enable_ldap" = "xyes"; then

  AC_ARG_WITH(ldap-dir, [  --with-ldap-dir=PFX   Location of LDAP], with_ldap_dir="$withval", with_ldap_dir="/usr")

  dnl Check for the includes presence
  AC_MSG_CHECKING(for LDAP includes in ${with_ldap_dir}/include/)
  AC_MSG_RESULT()


  if test -f ${with_ldap_dir}/include/ldap.h; then
  	LDAP_CFLAGS="-I${with_ldap_dir}/include"
  else
  	AC_MSG_ERROR(You need the LDAP headers to compile Ekiga with LDAP support)
  fi

  dnl Checking for the library presence
  LIBS_save="$LIBS"
  LIBS="${LIBS} -L${with_ldap_dir}/${libname}/ -llber"
  AC_CHECK_LIB(ldap, main, ldap_libs="yes", ldap_libs="no")
  LIBS="${LIBS_save}"

  if test "x${ldap_libs}" != "xno"; then
  	LDAP_LIBS="-llber -lldap"
  	if test "x${with_ldap_dir}" != "x/usr"; then
  		LDAP_LIBS="-L${with_ldap_dir}/${libname} $LDAP_LIBS"
  	fi
  else
  	AC_MSG_ERROR(You need the LDAP library to compile Ekiga with LDAP support)
  fi

  dnl Checking for libresolv
  if test ${gm_platform} = "linux" ; then
  	AC_CHECK_LIB(resolv, res_gethostbyaddr, [LIBS="-lresolv $LIBS"], AC_MSG_ERROR([*** libresolv not found.]), -lresolv)
  	LDAP_LIBS="${LDAP_LIBS} -lresolv"
  fi
  found_ldap="yes"
  AC_SUBST(LDAP_CFLAGS)
  AC_SUBST(LDAP_LIBS)
  LDAP="enabled"
  AC_DEFINE(HAVE_LDAP,1,[LDAP support])
fi

AM_CONDITIONAL(HAVE_LDAP, test "x$found_ldap" = "xyes")


AM_PROG_AS

dnl ###############################
dnl Checks for MMX support
dnl ###############################
AC_MSG_CHECKING(for x86 platform)
case $host_cpu in
  i386|i486|i586|i686|i786|k6|k7)
	use_x86_asm=yes
        ;;
   *)
  	use_x86_asm=no
esac
AC_MSG_RESULT($use_x86_asm)

use_mmx_asm=no
if test $use_x86_asm = yes; then
    save_ac_ext=$ac_ext
    ac_ext=S
    
    AC_MSG_CHECKING(compiler support for MMX)
    cp lib/pixops/scale_line_22_33_mmx.S conftest.S
    if AC_TRY_EVAL(ac_compile); then
        use_mmx_asm=yes
    fi

    rm -rf conftest*

    ac_ext=$save_ac_ext
    if test $use_mmx_asm = yes; then
      AC_DEFINE([USE_MMX], 1, [MMX Support])
      AC_MSG_RESULT(yes)
    else
      AC_MSG_RESULT(no)
    fi
fi

AM_CONDITIONAL(USE_MMX, test x$use_mmx_asm = xyes)



dnl ###############################
dnl Checks for Shared Memory extension
dnl ###############################
SHM="disabled"

if test "x${gm_platform}" = "xlinux" ; then
  AC_CHECK_LIB(Xext, XShmAttach, with_shm="yes", with_shm="no")
  if test "x${with_shm}" != "xno"; then
  SHM="enabled"
  AC_DEFINE(HAVE_SHM,1,[SHM Support])
  fi
fi
AM_CONDITIONAL(HAVE_SHM, test "x$SHM" = "xenabled")

dnl ###############################
dnl Checks for Xv extension
dnl ###############################
XV="disabled"
AC_ARG_ENABLE(xv, [  --disable-xv			Disable XVideo hardware acceleration],,enable_xv=yes)  
if test "x${gm_platform}" != "mingw" -a "x$enable_xv" = "xyes"; then
  PKG_CHECK_MODULES([XV], [xv], 
                    [AC_DEFINE(HAVE_XV,1,[XVideo support])
                     XV="enabled"
                    ])
  AC_SUBST(XV_CFLAGS)
  AC_SUBST(XV_LIBS)
fi

AM_CONDITIONAL(HAVE_XV, test "x$XV" = "xenabled")


dnl ###########################################################################
dnl Checks for DX extension
dnl ###########################################################################
DX="disabled"

if test "x${gm_platform}" = "xmingw" ; then
  AC_ARG_WITH(directx-dir, [  --with-directx-dir=PFX		Location of DirectDraw], with_directx_dir="$withval", with_directx_dir=".")

  dnl Check for the includes presence
  AC_MSG_CHECKING(for DirectX includes in ${with_directx_dir})
  AC_MSG_RESULT()
  if test -d ${with_directx_dir}; then
    DX_CFLAGS="-I${with_directx_dir}"
    DX_LIBS="$EKIGA_LIBS -lddraw"
    AC_SUBST(DX_LIBS)
    AC_SUBST(DX_CFLAGS)
    DX="enabled"
    AC_DEFINE(HAVE_DX,1,[DirectDraw support])
  else
    AC_MSG_ERROR([Unable to locate DirectX header files; --with-directx-dir is obligatory])
  fi
fi

AM_CONDITIONAL(HAVE_DX, test "x$DX" = "xenabled")


dnl ###############################
dnl   DBUS Support
dnl ###############################
DBUS="disabled"
DBUS_SERVICE="disabled"

if test "x${gm_platform}" != "xmingw" ; then
  AC_ARG_ENABLE(dbus, [  --disable-dbus		Disable DBUS support],,enable_dbus=yes)
  AC_ARG_ENABLE(dbus-service, [  --disable-dbus-service 	Disable DBUS service installation],,enable_dbus_service=yes)

  if test "x$enable_dbus" = "xyes"; then
    PKG_CHECK_MODULES([DBUS], [dbus-1 >= 0.36 dbus-glib-1 >= 0.36], [found_dbus=yes])
    AC_SUBST(DBUS_CFLAGS)
    AC_SUBST(DBUS_LIBS)
    DBUS="enabled"
    AC_DEFINE(HAVE_DBUS,1,[DBUS support])

    if test "x$found_dbus" = "xyes" -a "x$enable_dbus_service" = "xyes"; then
      DBUS_SERVICE="enabled"
    fi
  fi
fi

AM_CONDITIONAL(HAVE_DBUS, test "x$found_dbus" = "xyes")
AM_CONDITIONAL(INSTALL_DBUS_SERVICE, test "x$DBUS_SERVICE" = "xenabled")


dnl ###############################
dnl   V4L headers
dnl   These are only necessary until HAL has a decent V4L support...
dnl ###############################

V4L="disabled"
if test "x${win32}" != "x1"; then
  AC_CHECK_HEADER(linux/videodev.h, V4L="enabled")
  
  if test "x${V4L}" != "xdisabled"; then
    AC_DEFINE(HAVE_V4L,1,[V4L support])
  fi
fi
AM_CONDITIONAL(HAVE_V4L, test "x${V4L}" != "xdisabled")

dnl ###############################
dnl   Avahi / mDNS Support
dnl ###############################
AVAHI="disabled"

if test "x${gm_platform}" != "xmingw" ; then
  AC_ARG_ENABLE(avahi, [  --disable-avahi		Disable mDNS support],,enable_avahi=yes)

  if test "x$enable_avahi" = "xyes"; then
    PKG_CHECK_MODULES([AVAHI], [avahi-client >= 0.6 avahi-glib >= 0.6], [found_avahi=yes])
    AC_SUBST(AVAHI_CFLAGS)
    AC_SUBST(AVAHI_LIBS)
    AVAHI="enabled"
    AC_DEFINE(HAVE_AVAHI,1,[Avahi support])
  fi
fi

AM_CONDITIONAL(HAVE_AVAHI, test "x$found_avahi" = "xyes")


dnl ###############################
dnl   Gnome-Doc-Utils
dnl ###############################
GDU="disabled"
if test "x${gm_platform}" = "xmingw" ; then
  GNOME_DOC_INIT_ARG=:
fi

GNOME_DOC_INIT(,,[$GNOME_DOC_INIT_ARG])
if test "x${gm_platform}" != "xmingw" ; then
  AC_ARG_ENABLE(gdu, [  --disable-gdu			Disable GNOME-Doc-Utils support],,enable_gdu=yes)
  if test "x$enable_gdu" = "xyes"; then
    AC_PATH_PROG(SK_CONFIG,scrollkeeper-config,no)
    if test "x$SK_CONFIG" = "xno"; then
      AC_MSG_ERROR(Couldn't find scrollkeeper-config. Please install the scrollkeeper package: http://scrollkeeper.sourceforge.net)
    fi
    GDU="enabled"
  fi
fi

AM_CONDITIONAL(HAVE_GNOME_DOC_UTILS, test "x${enable_gdu}" = "xyes")

                                                                                  
dnl ###########################################################################
dnl PTLib + OPAL Headers and Libraries
dnl ###########################################################################
PKG_CHECK_MODULES([PTLIB], [ptlib >= ${PTLIB_REC_VERSION}])
PKG_CHECK_MODULES([OPAL], [opal >= ${OPAL_REC_VERSION}])

SUFFIX=
AC_ARG_ENABLE([debug],
              [AC_HELP_STRING([--enable-debug],[Link to debug versions of opal and ptlib (opal_d and ptlib_d)])],
              [if test "x$enableval" = "xyes"; then 
                SUFFIX="_d"
              fi])

AC_ARG_ENABLE([static-libs],
              [AC_HELP_STRING([--enable-static-libs],[Link to opal and ptlib static libraries])],
              [if test "x$enableval" = "xyes"; then 
                SUFFIX="${SUFFIX}_s"
               fi
              ])

if test "x$SUFFIX" != x; then 
    OPAL_LIBS=`$PKG_CONFIG opal --define-variable=suffix=$SUFFIX --libs`
    PTLIB_LIBS=`$PKG_CONFIG ptlib --define-variable=suffix=$SUFFIX --libs`
fi

AC_SUBST(PTLIB_CFLAGS)
AC_SUBST(PTLIB_LIBS)
AC_SUBST(OPAL_CFLAGS)
AC_SUBST(OPAL_LIBS)

dnl The =1 checks are here only for backward compatibility
H323=`$PKG_CONFIG --variable=h323 opal`  
if test "x$H323" = "x1"; then
  H323="yes"
else
  H323=`$PKG_CONFIG --variable=OPAL_H323 opal`  
fi

if test "x$H323" = "xyes"; then
  AC_DEFINE(HAVE_H323,1,[H323 support])
fi
AM_CONDITIONAL(HAVE_H323, test "x${H323}" = "xyes")

SIP=`$PKG_CONFIG --variable=sip opal`  
if test "x$SIP" = "x1"; then
  SIP="yes"
else
  SIP=`$PKG_CONFIG --variable=OPAL_SIP opal`
fi

if test "x$SIP" = "xyes"; then
  AC_DEFINE(HAVE_SIP,1,[SIP support])
fi
AM_CONDITIONAL(HAVE_SIP, test "x${SIP}" = "xyes")


dnl #########################################################################
dnl  Support for internationalization
dnl ########################################################################
GETTEXT_PACKAGE=AC_PACKAGE_NAME
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE",[fix])
AM_GLIB_GNU_GETTEXT


dnl #########################################################################
dnl Set PACKAGE_LOCALE_DIR in config.h.
dnl #########################################################################
if test "x${prefix}" = "xNONE"; then
  AC_DEFINE_UNQUOTED(PACKAGE_LOCALE_DIR, "${ac_default_prefix}/${DATADIRNAME}/locale",[fix])
else
  AC_DEFINE_UNQUOTED(PACKAGE_LOCALE_DIR, "${prefix}/${DATADIRNAME}/locale",[fix])
fi

AH_BOTTOM(
#ifndef _
#include <libintl.h>
#define _(x) gettext(x)
#ifdef gettext_noop
#define N_(String) gettext_noop (String)
#else
#define N_(String) (String)
#endif
#endif
)


dnl ###########################################################################
dnl  Data and configuration directories for the system
dnl ###########################################################################
gnomedatadir=`eval "echo ${datadir}"`
gnomeconfdir=`eval "echo ${sysconfdir}"`
AC_SUBST(gnomedatadir)
AC_SUBST(gnomeconfdir)

if test "x${prefix}" = "xNONE"; then
  AC_DEFINE_UNQUOTED(DATA_DIR, "${ac_default_prefix}/${DATADIRNAME}",[fix])
else
  AC_DEFINE_UNQUOTED(DATA_DIR, "${prefix}/${DATADIRNAME}",[fix])
fi

dnl ###########################################################################
dnl  Output the different Makefiles
dnl ###########################################################################
AC_OUTPUT(
Makefile
Doxyfile
src/AC_PACKAGE_NAME-config-tool
AC_PACKAGE_NAME.schemas.in
AC_PACKAGE_NAME.desktop.in
lib/Makefile
lib/gmconf/Makefile
lib/gui/Makefile
lib/pixops/Makefile
lib/toolbox/Makefile
lib/platform/Makefile
po/Makefile.in
man/Makefile
src/Makefile
pixmaps/Makefile
sounds/Makefile
help/Makefile
lib/engine/Makefile
lib/engine/account/Makefile
lib/engine/account/skel/Makefile
lib/engine/addressbook/Makefile
lib/engine/addressbook/call-history/Makefile
lib/engine/addressbook/evolution/Makefile
lib/engine/addressbook/ldap/Makefile
lib/engine/addressbook/skel/Makefile
lib/engine/chat/Makefile
lib/engine/chat/echo/Makefile
lib/engine/chat/skel/Makefile
lib/engine/framework/Makefile
lib/engine/presence/Makefile
lib/engine/presence/avahi/Makefile
lib/engine/presence/local-roster/Makefile
lib/engine/presence/skel/Makefile
lib/engine/protocol/Makefile
lib/engine/protocol/skel/Makefile
lib/engine/videooutput/Makefile
lib/engine/videooutput/skel/Makefile
lib/engine/videooutput/x/Makefile
lib/engine/videooutput/dx/Makefile
lib/engine/videoinput/Makefile
lib/engine/videoinput/skel/Makefile
lib/engine/videoinput/mlogo/Makefile
lib/engine/videoinput/ptlib/Makefile
lib/engine/audioinput/Makefile
lib/engine/audioinput/skel/Makefile
lib/engine/audioinput/null/Makefile
lib/engine/audioinput/ptlib/Makefile
lib/engine/audiooutput/Makefile
lib/engine/audiooutput/skel/Makefile
lib/engine/audiooutput/null/Makefile
lib/engine/audiooutput/ptlib/Makefile
lib/engine/hal/Makefile
lib/engine/hal/skel/Makefile
lib/engine/hal/dbus/Makefile
lib/engine/gui/Makefile
lib/engine/gui/gtk-core/Makefile
lib/engine/gui/gtk-frontend/Makefile
lib/engine/components/Makefile
lib/engine/components/gmconf-personal-details/Makefile
lib/engine/components/avahi-publisher/Makefile
)


dnl ###########################################################################
dnl Summary
dnl ###########################################################################

echo ""
echo "================ Final configuration ==================="
echo "          Installing into prefix  :  $prefix"
echo ""
if test "x${gm_platform}" != "xmingw" ; then
echo "                   GNOME support  :  $GNOME"
echo "                   GConf support  :  $GCONF"
echo "   Evolution-Data-Server support  :  $EDS"
echo "                  NOTIFY support  :  $NOTIFY"
fi
echo "                    LDAP support  :  $LDAP"
echo ""
echo "                  XVideo support  :  $XV"
if test "x${gm_platform}" = "xmingw" ; then
echo "                 DirectX support  :  $DX"
fi
echo ""
echo "                   H.323 support  :  $H323"
echo "                     SIP support  :  $SIP"
echo ""
if test "x${gm_platform}" != "xmingw" ; then
echo "                    DBUS support  :  $DBUS"
echo "            DBUS service support  :  $DBUS_SERVICE"
echo "             mDNS/DNS-SD support  :  $AVAHI"
fi
echo ""
echo "                         OS Type  :  $target_os"
echo "                    Machine Type  :  $target_cpu"
echo "                      Byte Order  :  $byte_order"
echo ""
echo " If all settings are OK, type make and make install "
echo "========================================================"
