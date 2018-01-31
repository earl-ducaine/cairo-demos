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

void
draw_spline (cairo_t *cr, double height);

void
draw_splines (cairo_t *cr, int width, int height);

#define WIDTH 600
#define HEIGHT 300
#define STRIDE (WIDTH * 4)

unsigned char image[STRIDE*HEIGHT];

int
main (void)
{
    cairo_surface_t *surface;
    cairo_t *cr;

    surface = cairo_image_surface_create_for_data (image, CAIRO_FORMAT_ARGB32,
						   WIDTH, HEIGHT, STRIDE);

    cr = cairo_create (surface);

    cairo_rectangle (cr, 0, 0, WIDTH, HEIGHT);
    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_fill (cr);

    draw_splines (cr, WIDTH, HEIGHT);

    cairo_surface_write_to_png (surface, "splines_tolerance.png");

    cairo_destroy (cr);

    cairo_surface_destroy (surface);

    return 0;
}

void
draw_spline (cairo_t *cr, double height)
{
    cairo_move_to (cr, 0, .1 * height);
    height = .8 * height;
    cairo_rel_curve_to (cr,
		  -height/2, height/2,
		  height/2, height/2,
		  0, height);
    cairo_stroke (cr);
}

void
draw_splines (cairo_t *cr, int width, int height)
{
    int i;
    double tolerance[5] = {.1, .5, 1, 5, 10};
    double line_width = .08 * width;
    double gap = width / 6;

    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_set_line_width (cr, line_width);

    cairo_translate (cr, gap, 0);
    for (i=0; i < 5; i++) {
	cairo_set_tolerance (cr, tolerance[i]);
	draw_spline (cr, height);
	cairo_translate (cr, gap, 0);
    }
}
