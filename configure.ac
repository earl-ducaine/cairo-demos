

AC_INIT([png], 1.0)

PKG_CHECK_MODULES(CAIRO, cairo)
PKG_CHECK_MODULES([GTK], [gtk+-3.0])
PKG_CHECK_MODULES([RSVG], [librsvg-2.0])



AC_CHECK_LIB([X11], [XClearWindow])
AC_CHECK_LIB([xcb], [xcb_get_setup])
AC_CHECK_LIB([cairo], [cairo_image_surface_create_for_data])
AC_CHECK_LIB([m], [floor])
AC_CONFIG_AUX_DIR(aux)



AM_INIT_AUTOMAKE([foreign -Wall -Werror])

AC_CONFIG_FILES([Makefile png/Makefile X11/Makefile cairo-candy/Makefile])
AC_OUTPUT
