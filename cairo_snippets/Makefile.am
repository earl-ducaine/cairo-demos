INCLUDES = @SNIPPETS_CFLAGS@

bin_PROGRAMS =					\
	cairo_snippets_png			\
	cairo_snippets_ps			\
	cairo_snippets_html

if BUILD_GTKCAIRO
bin_PROGRAMS += cairo_snippets_gtk
endif

if BUILD_PDF
bin_PROGRAMS += cairo_snippets_pdf
endif

if BUILD_WIN32
bin_PROGRAMS += cairo_snippets_win32
endif

noinst_PROGRAMS =				\
	prepare_snippets

common_sources = snippets.c

cairo_snippets_gtk_SOURCES = $(common_sources) cairo_snippets_gtk.c
cairo_snippets_gtk_LDADD=@SNIPPETS_LIBS@ @GTKCAIRO_LIBS@
cairo_snippets_gtk_CFLAGS=@GTKCAIRO_CFLAGS@

cairo_snippets_png_SOURCES = $(common_sources) cairo_snippets_png.c
cairo_snippets_png_LDADD=@SNIPPETS_LIBS@

cairo_snippets_ps_SOURCES = $(common_sources) cairo_snippets_ps.c
cairo_snippets_ps_LDADD=@SNIPPETS_LIBS@

cairo_snippets_html_SOURCES = $(common_sources) cairo_snippets_html.c
cairo_snippets_html_LDADD=@SNIPPETS_LIBS@

cairo_snippets_pdf_SOURCES = $(common_sources) cairo_snippets_pdf.c
cairo_snippets_pdf_LDADD=@SNIPPETS_LIBS@

cairo_snippets_win32_SOURCES = $(common_sources) cairo_snippets_win32.c
cairo_snippets_win32_LDADD=@SNIPPETS_LIBS@ -lgdi32

prepare_snippets_SOURCES = prepare_snippets.c

AM_CFLAGS = -g -Wall

snippets.c : prepare_snippets snippets/*.cairo
	./prepare_snippets snippets/*.cairo

BUILT_SOURCES = snippets.c
CLEANFILES = *.html snippets.ps snippets.pdf *.png *.eps snippets.c

## webpage at http://cairographics.org/samples/

html : pngs snippets.html
	cp html/cairo_snippets.css snippets

snippets.html : cairo_snippets_html
	./cairo_snippets_html

scp : html
	scp *.png *.html *.css freedesktop.org:/srv/cairo.freedesktop.org/www/samples

pngs : cairo_snippets_png
	./cairo_snippets_png

## for compiling eps' includeable from LaTeX

epss : pngs
	for a in *.png; do \
	   pngtopnm $$a | pnmtops -noturn -equalpixels > `echo $$a | sed s/png/eps/`;\
	done
