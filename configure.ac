dnl Process this file with autoconf to produce a configure script.
dnl $Id$

# Init.
AC_PREREQ(2.57)
AC_INIT(libTiCables, 3.8.5, [tilp-users@lists.sourceforge.net])

MAJOR_VERSION=3
MINOR_VERSION=8
MICRO_VERSION=5

LT_CURRENT=`expr $MINOR_VERSION + $MAJOR_VERSION`
LT_REVISION=$MICRO_VERSION
LT_AGE=$MINOR_VERSION

dnl Library version.
dnl current:revision:age
dnl  * On a new release, increment 'revision'.
dnl  * If you added things to the api, increment 'current',
dnl    reset 'revision' and increment 'age'.
dnl  * If you changed/removed things from the api, increment 'current',
dnl    reset 'revision' and reset 'age'.
AC_SUBST(LT_LIBVERSION, $LT_CURRENT:$LT_REVISION:$LT_AGE)

AM_INIT_AUTOMAKE([dist-bzip2])
AM_MAINTAINER_MODE

AC_PREFIX_DEFAULT(/usr)

dnl Files to configure.
AC_CONFIG_SRCDIR([src/intrface.c])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_FILES([
  Makefile
  build/Makefile
  intl/Makefile
  po/Makefile.in
  src/Makefile
  tests/Makefile
  libticables.lsm
  libticables.spec
  ticables.pc
])

# Setup libtool.
AC_DISABLE_STATIC
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_LN_S
AC_ISC_POSIX
AC_PROG_AWK
AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
if test x$PKG_CONFIG = xno ; then
  AC_MSG_ERROR([*** pkg-config not found. See http://www.freedesktop.org/Software/pkgconfig/])
fi

# Checks for libraries.
AM_GNU_GETTEXT
AM_GNU_GETTEXT_VERSION(0.13.1)
AC_CHECK_PROG(LIBUSB_CONFIG, libusb-config, yes, no)
if test x$LIBUSB_CONFIG = xyes; then
  LIBUSB_CFLAGS=`libusb-config --cflags`
  LIBUSB_LIBS=`libusb-config --libs`
  AC_DEFINE(HAVE_LIBUSB, 1, [Define to 1 to enable libusb support.])
else
  LIBUSB_CFLAGS=
  LIBUSB_LIBS=
fi
AC_SUBST(LIBUSB_CFLAGS)
AC_SUBST(LIBUSB_LIBS)

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([asm/io.h errno.h fcntl.h inttypes.h stdint.h stdlib.h string.h strings.h sys/file.h sys/ioctl.h sys/ipc.h sys/perm.h sys/shm.h sys/stat.h sys/time.h sys/types.h termios.h unistd.h])
AC_CHECK_HEADERS([linux/ticable.h linux/tiglusb.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_UID_T
AC_TYPE_MODE_T
AC_C_CONST
AC_HEADER_TIME

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_STAT
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([bzero cfmakeraw gettimeofday memset mkfifo select strdup strerror strstr uname])

# Logging
AC_ARG_ENABLE(logging,
  AC_HELP_STRING([--enable-logging], [enable logging]),
  [cab_cv_enable_logging=$enableval], [])
AC_CACHE_CHECK([whether to enable logging],
  [cab_cv_enable_logging], [cab_cv_enable_logging=no])
if test x$cab_cv_enable_logging = xyes; then
  AC_DEFINE(ENABLE_LOGGING, 1, [Define to 1 to enable logging.])
fi

# Platform specific tests.
dnl AC_CANONICAL_HOST
case "$host" in
  i386-*-*bsd*)          ARCH="-D__BSD__ -D__I386__" ;;
  *-*-*bsd*)             ARCH="-D__BSD__" ;;
  alpha*-*-linux-*)      ARCH="-D__ALPHA__ -D__LINUX__" ;;
  alpha*-*-*-*)          ARCH="-D__ALPHA__ -D__LINUX__" ;;
  arm*-*-linux-*)        ARCH="-D__ARM__ -D__LINUX__" ;;
  hppa*)                 ARCH="-D__LINUX__" ;;
  i[[3456]]86-*-linux-*) ARCH="-D__I386__ -D__LINUX__" ;;
  i[[3456]]86-*-hurd-*)  ARCH="-D__LINUX__" ;;
  i[[3456]]86-*-mingw*)  ARCH="-D__WIN32__ -D__MINGW32__" ;;
  i[[3456]]86-*-cygwin)  ARCH="-D__WIN32__ -D__CYGWIN__ -mno-cygwin" ;;
  ia64-*-linux-*)        ARCH="-D__LINUX__" ;;
  m68k-*-linux-*)        ARCH="-D__M68K__ -D__LINUX__" ;;
  mips-*-linux-*)        ARCH="-D__MIPS__ -D__LINUX__" ;;
  mipsel-*-linux-*)      ARCH="-D__MIPS__ -D__LINUX__" ;;
  powerpc-*-linux-*)     ARCH="-D__PPC__  -D__LINUX__" ;;
  s390-*-linux-*)        ARCH="-D__LINUX__" ;;
  sh-*-linux-*)          ARCH="-D__LINUX__" ;;
  sparc-*-linux-*)       ARCH="-D__SPARC__ -D__LINUX__" ;;
  sparc*-sun-solaris*)   ARCH="-D__SPARC__ -D__SOL__" ;;
esac
CFLAGS="$CFLAGS $ARCH"

# Ensure MSVC-compatible struct packing convention is used when
# compiling for Win32 with gcc.
# What flag to depends on gcc version: gcc3 uses "-mms-bitfields", while
# gcc2 uses "-fnative-struct".
case $host_os in
*mingw*|*cygwin*)
  if test x$GCC = xyes; then
    msnative_struct=''
    AC_MSG_CHECKING([how to get MSVC-compatible struct packing])
    if test -z "$ac_cv_prog_CC"; then
      our_gcc="$CC"
    else
      our_gcc="$ac_cv_prog_CC"
    fi
    case `$our_gcc --version | sed -e 's,\..*,.,' -e q` in
      2.)
        if $our_gcc -v --help 2>/dev/null | grep fnative-struct >/dev/null; then
          msnative_struct='-fnative-struct'
        fi
        ;;
      *)
        if $our_gcc -v --help 2>/dev/null | grep ms-bitfields >/dev/null; then
          msnative_struct='-mms-bitfields'
        fi
        ;;
    esac
    if test x"$msnative_struct" = x ; then
      AC_MSG_RESULT([no way])
      AC_MSG_WARN([produced libraries might be incompatible with MSVC-compiled code])
    else
      CFLAGS="$CFLAGS $msnative_struct"
      AC_MSG_RESULT([${msnative_struct}])
    fi
  fi
  ;;
esac

# Output.
AC_OUTPUT

echo "Now, you can type 'make' and 'make install'."

if test x$enable_logging = xyes; then
 echo ""
 echo "Logging has been enabled. Log files are generated in a user's"
 echo "home directory."
 echo ""
else
 echo ""
 echo "Developpers or users who want to send a bug report can add the" 
 echo "'--enable-logging' option on the 'configure' command line to" 
 echo "enable logging. Log files are generated in a user's home directory."
 echo ""
fi