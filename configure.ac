#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(ample, 0.0.3, clb@nada.kth.se)
AC_CONFIG_SRCDIR([src/AmpleNode.cpp])
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER([include/config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB

# Checks for libraries.
AC_ARG_WITH([verse], [AC_HELP_STRING([--with-verse], [location of verse])],
[ if test ! -d "$withval" -o ! -r "$withval/verse.h"; then
    AC_MSG_ERROR([invalid location "$withval"])
  fi
  CPPFLAGS="-I$withval $CPPFLAGS"
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

