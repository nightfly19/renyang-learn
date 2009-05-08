# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111, USA.
# 
# Process this file with autoconf to produce a configure script.

AC_INIT(ihu, 0.6.0, mtrotta@users.sourceforge.net)
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(src/config.h)

AC_LANG_CPLUSPLUS
AC_PROG_CXX
AC_PROG_INSTALL

BNV_HAVE_QT

if test "$have_qt" = "no";
then
	AC_MSG_ERROR([QT LIBRARIES NOT FOUND. They might be in a non-standard path (then use --with-Qt-dir option), or they need to be installed.])
fi


AC_CHECK_LIB(gmpxx, __gmpz_init,, [AC_MSG_ERROR([GNU MP not found, download at http://swox.com/gmp])])
AC_CHECK_LIB(asound, snd_pcm_writei,, [AC_MSG_ERROR(ALSA needed! Download at http://www.alsa-prject.org)])
AC_CHECK_LIB(jack, jack_client_new, [JACK=yes], [JACK=no])
AC_CHECK_LIB(speex, speex_encode,, [AC_MSG_ERROR(Speex needed! Download at http://www.speex.org)])
AC_CHECK_HEADER([soundtouch/SoundTouch.h],, [AC_MSG_ERROR(Soundtouch needed! Download at http://www.surina.net/soundtouch/)])
XIPH_PATH_OGG(,AC_MSG_ERROR(Ogg needed! Download the latest version of libogg at http://www.xiph.org))

if test "$JACK" = "yes"; then
AC_MSG_CHECKING(whether to enable Jack)
jack_default="no"
AC_ARG_ENABLE(jack, [  --disable-jack   Disable JACK sound driver (if available)],, disable_jack=$jack_default)

if test "$disable_jack" = "no"; then
	AC_DEFINE([HAVE_LIBJACK], [], [])
	LIBS="$LIBS -ljack"
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
fi
fi
 
AC_OUTPUT(Makefile doc/Makefile icon/Makefile)

echo -e -n "\nCompiling with Jack sound driver support: "
if test "$disable_jack" = "no";
then
	echo "yes"
else
	echo "no"
fi

echo -e "Ok, type \"make; make install\" to compile and install.";
