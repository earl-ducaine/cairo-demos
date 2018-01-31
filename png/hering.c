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
#include <math.h>

void
draw_hering (cairo_t *cr, int width, int height);

#define WIDTH 300
#define HEIGHT 600
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

    draw_hering (cr, WIDTH, HEIGHT);

    cairo_surface_write_to_png (surface, "hering.png");

    cairo_destroy (cr);

    cairo_surface_destroy (surface);

    return 0;
}

void
draw_hering (cairo_t *cr, int width, int height)
{
#define LINES 32
#define MAX_THETA (.80 * M_PI_2)
#define THETA_INC (2 * MAX_THETA / (double) (LINES-1))

    int i;

    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_set_line_width (cr, 2.0);

    cairo_save (cr);
    {
	cairo_translate (cr, width / 2, height / 2);
	cairo_rotate (cr, MAX_THETA);
	
	for (i=0; i < LINES; i++) {
	    cairo_move_to (cr, -2 * width, 0);
	    cairo_line_to (cr, 2 * width, 0);
	    cairo_stroke (cr);
	    
	    cairo_rotate (cr, - THETA_INC);
	}
    }
    cairo_restore (cr);

    cairo_set_line_width (cr, 6);
    cairo_set_source_rgb (cr, 1, 0, 0);

    cairo_move_to (cr, width / 4, 0);
    cairo_rel_line_to (cr, 0, height);
    cairo_stroke (cr);

    cairo_move_to (cr, 3 * width / 4, 0);
    cairo_rel_line_to (cr, 0, height);
    cairo_stroke (cr);
}
