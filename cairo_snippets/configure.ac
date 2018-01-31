AC_INIT(snippets.h)

AM_INIT_AUTOMAKE(cairo_snippets, 0.1)
AC_CONFIG_HEADERS(config.h)

AM_MAINTAINER_MODE
AC_PROG_CC
AC_PROG_CPP
AC_STDC_HEADERS

PKG_CHECK_MODULES(SNIPPETS, libpng libsvg-cairo >= 0.1.2 cairo >= 0.3.0)
PKG_CHECK_MODULES(GTKCAIRO, gtkcairo, enable_gtkcairo=yes, enable_gtkcairo=no)

AM_CONDITIONAL(BUILD_GTKCAIRO, test x$enable_gtkcairo = xyes)

AC_MSG_CHECKING(For Cairo Win32 backend)
enable_win32=no
snippets_save_LDFLAGS=$LDFLAGS
LDFLAGS="$LDFLAGS $SNIPPETS_LIBS"
AC_CHECK_LIB(cairo, cairo_win32_surface_create, enable_win32=yes, :)
LDFLAGS=$snippets_save_LDFLAGS
AC_MSG_RESULT($enable_win32)

AM_CONDITIONAL(BUILD_WIN32, test $enable_win32 = yes)

AC_MSG_CHECKING(For Cairo PDF backend)
enable_pdf=no
snippets_save_LDFLAGS=$LDFLAGS
LDFLAGS="$LDFLAGS $SNIPPETS_LIBS"
AC_CHECK_LIB(cairo, cairo_pdf_surface_create, enable_pdf=yes, :)
LDFLAGS=$snippets_save_LDFLAGS
AC_MSG_RESULT($enable_pdf)

AM_CONDITIONAL(BUILD_PDF, test $enable_pdf = yes)

AC_OUTPUT(Makefile)
