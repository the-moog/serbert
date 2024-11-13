#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.71])
AC_INIT([SERBERT],[0.3.1],[https://github.com/the-moog/serbert])
AM_INIT_AUTOMAKE()
AC_CONFIG_SRCDIR([serbert.c])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.

AC_PROG_EGREP

AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([fcntl.h limits.h stdlib.h string.h sys/ioctl.h sys/time.h termios.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T


AC_STRUCT_TM

# Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_FUNC_SELECT_ARGTYPES

AC_CACHE_CHECK([return type of signal handlers],[ac_cv_type_signal],[AC_COMPILE_IFELSE(
[AC_LANG_PROGRAM([#include <sys/types.h>
#include <signal.h>
],
		 [return *(signal (0, 0)) (0) == 1;])],
		   [ac_cv_type_signal=int],
		   [ac_cv_type_signal=void])])
AC_DEFINE_UNQUOTED([RETSIGTYPE],[$ac_cv_type_signal],[Define as the return type of signal handlers
		    (`int' or `void').])

AC_FUNC_STRFTIME
AC_FUNC_STRTOD
AC_CHECK_FUNCS([gettimeofday memmove select strerror strrchr strtol strtoul])

AC_CONFIG_FILES([Makefile
                 doc/Makefile])
AC_OUTPUT
