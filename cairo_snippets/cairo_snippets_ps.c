/* ps frontend for cairo_snippets
 * (c) Øyvind Kolås 2004, placed in the public domain
 */

#include "snippets.h"
#include <cairo-ps.h>

#define X_INCHES 2
#define Y_INCHES 2
#define X_PPI    300.0
#define Y_PPI    300.0

#define WIDTH    X_INCHES * 72.0
#define HEIGHT   Y_INCHES * 72.0

/* add a page with the specified snippet to the output ps file */
static void
write_page (cairo_t *cr,
            int      no);

int
main (void)
{
        int i;
	cairo_surface_t *surface;
        cairo_t *cr;

	surface = cairo_ps_surface_create ("snippets.ps", WIDTH, HEIGHT);
	cr = cairo_create (surface);
	
        for (i=0;i<snippet_count;i++)
             write_page (cr, i);

        cairo_destroy (cr);
	cairo_surface_destroy (surface);

        return 0;
}

static void
write_page (cairo_t *cr,
            int      no)
{
        fprintf (stdout, "processing %s", snippet_name[no]);

        cairo_save (cr);
          snippet_do (cr, no, WIDTH, HEIGHT);
          cairo_show_page (cr);
        cairo_restore (cr);

        fprintf (stdout, "\n");
}
