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
draw_outlines (cairo_t *cr, int surface_width, int surface_height);

void
draw_flat (cairo_t *cr, double width, double height);

void
draw_tent (cairo_t *cr, double width, double height);

void
draw_cylinder (cairo_t *cr, double width, double height);

cairo_pattern_t *
create_gradient (double width, double height);

#define WIDTH 750
#define HEIGHT 500
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

    draw_outlines (cr, WIDTH, HEIGHT);

    cairo_surface_write_to_png (surface, "outline.png");

    cairo_destroy (cr);

    cairo_surface_destroy (surface);

    return 0;
}

cairo_pattern_t *
create_gradient (double width, double height)
{
    cairo_pattern_t *gradient;

    gradient = cairo_pattern_create_linear (0, 0, width, 0);

    cairo_pattern_add_color_stop_rgb (gradient, 0.0,
				      0., 0., 0.);

    cairo_pattern_add_color_stop_rgb (gradient, 0.5,
				      1., 1., 1.);

    cairo_pattern_add_color_stop_rgb (gradient, 1.0,
				      0., 0., 0.);

    return gradient;
}

void
draw_outlines (cairo_t *cr, int surface_width, int surface_height)
{
    cairo_pattern_t *gradient;
    double width, height, pad;

    width = surface_width / 4.0;
    pad = (surface_width - (3 * width)) / 2.0;
    height = surface_height;

    gradient = create_gradient (width, height);

    cairo_set_source (cr, gradient);
    draw_flat (cr, width, height);

    cairo_translate (cr, width + pad, 0);
    cairo_set_source (cr, gradient);
    draw_tent (cr, width, height);

    cairo_translate (cr, width + pad, 0);
    cairo_set_source (cr, gradient);
    draw_cylinder (cr, width, height);

    cairo_restore (cr);

    cairo_pattern_destroy (gradient);
}

void
draw_flat (cairo_t *cr, double width, double height)
{
    double hwidth = width / 2.0;

    cairo_rectangle (cr, 0, hwidth, width, height - hwidth);

    cairo_fill (cr);
}

void
draw_tent (cairo_t *cr, double width, double height)
{
    double hwidth = width / 2.0;

    cairo_move_to    (cr,       0,  hwidth);
    cairo_rel_line_to (cr,  hwidth, -hwidth);
    cairo_rel_line_to (cr,  hwidth,  hwidth);
    cairo_rel_line_to (cr,       0,  height - hwidth);
    cairo_rel_line_to (cr, -hwidth, -hwidth);
    cairo_rel_line_to (cr, -hwidth,  hwidth);
    cairo_close_path (cr);

    cairo_fill (cr);
}

void
draw_cylinder (cairo_t *cr, double width, double height)
{
    double hwidth = width / 2.0;

    cairo_move_to (cr, 0, hwidth);
    cairo_rel_curve_to (cr,
		      0, -hwidth,
		  width, -hwidth,
		  width, 0);
    cairo_rel_line_to (cr, 0, height - hwidth);
    cairo_rel_curve_to (cr,
		       0, -hwidth,
		  -width, -hwidth,
		  -width, 0);
    cairo_close_path (cr);

    cairo_fill (cr);
}
