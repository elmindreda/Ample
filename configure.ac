#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(ample, 0.0.3, clb@nada.kth.se)
AC_CONFIG_SRCDIR([src/AmpleNode.cpp])
AM_INIT_AUTOMAKE([check-news])
AM_CONFIG_HEADER([include/config.h])
AC_CANONICAL_HOST

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB

# Check for platform profile and set up OpenGL build flags.
AC_MSG_CHECKING([for a suitable OpenGL profile])
case $host in
  *-*-mingw32* | *-*-cygwin*)
    AC_CHECK_LIB([glut], [glutInit], [], [AC_MSG_ERROR([GLUT not found])])
    LDFLAGS="-mwin32 -lopengl32 -lglu32 $LDFLAGS"
    AC_MSG_RESULT([Win32/WGL])
    ;;
  *-*-darwin* | *-*-macos10*)
    LDFLAGS="-framework GLUT -framework OpenGL -framework Cocoa $LDFLAGS"
    AC_MSG_RESULT([Carbon/AGL])
    ;;
  *)
    if test -d /usr/X11R6/include ; then
      CPPFLAGS="-I/usr/X11R6/include $CPPFLAGS"
    fi
    if test -d /usr/X11R6/lib ; then
      LDFLAGS="-L/usr/X11R6/lib $LDFLAGS"
    fi
    AC_CHECK_LIB([glut], [glutInit], [], [AC_MSG_ERROR([GLUT not found])])
    LDFLAGS="-lGL -lGLU -lX11 -lpthread $LDFLAGS"
    AC_MSG_RESULT([POSIX/GLX])
    AC_CHECK_LIB([Xxf86vm], [XF86VidModeQueryExtension])
    ;;
esac

# Checks for libraries.
AC_ARG_WITH([verse], [AC_HELP_STRING([--with-verse], [location of verse])],
[ if test ! -d "$withval" -o ! -r "$withval/verse.h"; then
    AC_MSG_ERROR([invalid location "$withval"])
  fi
  CPPFLAGS="-I$withval $CPPFLAGS"
  LDFLAGS="-L$withval $LDFLAGS"
])

# Checks for header files.
AC_CHECK_HEADERS([verse.h], [], [AC_MSG_ERROR([header not found])])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST

# Checks for library functions.
AC_CHECK_LIB([verse], [verse_send_connect], [], [AC_MSG_ERROR([library not found])])

AC_OUTPUT(
  Makefile
  docs/Makefile
  examples/Makefile
  include/Makefile
  src/Makefile)

