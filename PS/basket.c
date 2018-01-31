/*
 * Copyright © 2003 USC, Information Sciences Institute
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of the
 * University of Southern California not be used in advertising or
 * publicity pertaining to distribution of the software without
 * specific, written prior permission. The University of Southern
 * California makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 *
 * THE UNIVERSITY OF SOUTHERN CALIFORNIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL THE UNIVERSITY OF
 * SOUTHERN CALIFORNIA BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Carl D. Worth <cworth@isi.edu>
 */

#include <cairo.h>
#include <cairo-ps.h>
#include <math.h>

void
draw (cairo_t *cr);

#define X_INCHES	8
#define Y_INCHES	3

#define FILENAME "basket.ps"

int
main (void)
{
    cairo_surface_t *surface;
    cairo_t *cr;
    FILE *file;

    file = fopen (FILENAME, "w");
    if (file == NULL) {
	fprintf (stderr, "Failed to open file %s for writing.\n", FILENAME);
	return 1;
    }

    surface = cairo_ps_surface_create (FILENAME,
				       X_INCHES * 72.0,
				       Y_INCHES * 72.0);

    cr = cairo_create (surface);
    cairo_surface_destroy(surface);

    draw (cr);
    cairo_show_page (cr);

    cairo_destroy (cr);

    fclose (file);

    return 0;
}

void
draw (cairo_t *cr)
{
    cairo_move_to (cr, 50, 50);
    cairo_line_to (cr, 550, 50);
    cairo_curve_to (cr, 450, 240, 150, 240, 50, 50);
    cairo_close_path (cr);

    cairo_save (cr);
    cairo_set_source_rgb (cr, .8, .1, .1);
    cairo_fill_preserve (cr);
    cairo_restore (cr);

    cairo_set_line_width (cr, 6);
    cairo_set_source_rgb (cr, 0., 0., 0.);
    cairo_stroke (cr);
}
