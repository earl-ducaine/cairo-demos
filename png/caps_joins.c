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
draw_caps_joins (cairo_t *cr, int width, int height);

void
stroke_v_twice (cairo_t *cr, int dx, int dy);

void
draw_three_vs (cairo_t *cr, int width, int height, int line_width);

#define WIDTH 600
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

    draw_caps_joins (cr, WIDTH, HEIGHT);

    cairo_surface_write_to_png (surface, "caps_joins.png");

    cairo_destroy (cr);

    cairo_surface_destroy (surface);

    return 0;
}

void
stroke_v_twice (cairo_t *cr, int width, int height)
{
    cairo_move_to (cr, 0, 0);
    cairo_rel_line_to (cr, width / 2, height / 2);
    cairo_rel_line_to (cr, width / 2, - height / 2);

    cairo_save (cr);
    cairo_stroke (cr);
    cairo_restore (cr);

    cairo_save (cr);
    {
	cairo_set_line_width (cr, 2.0);
	cairo_set_line_cap (cr, CAIRO_LINE_CAP_BUTT);
	cairo_set_source_rgb (cr, 1, 1, 1);
	cairo_stroke (cr);
    }
    cairo_restore (cr);

    cairo_new_path (cr);
}

void
draw_caps_joins (cairo_t *cr, int width, int height)
{
    int line_width = height / 12 & (~1);

    cairo_set_line_width (cr, line_width);
    cairo_set_source_rgb (cr, 0, 0, 0);

    cairo_translate (cr, line_width, line_width);
    width -= 2 *line_width;

    cairo_set_line_join (cr, CAIRO_LINE_JOIN_BEVEL);
    cairo_set_line_cap (cr, CAIRO_LINE_CAP_BUTT);
    stroke_v_twice (cr, width, height);

    cairo_translate (cr, 0, height / 4 - line_width);
    cairo_set_line_join (cr, CAIRO_LINE_JOIN_MITER);
    cairo_set_line_cap (cr, CAIRO_LINE_CAP_SQUARE);
    stroke_v_twice (cr, width, height);

    cairo_translate (cr, 0, height / 4 - line_width);
    cairo_set_line_join (cr, CAIRO_LINE_JOIN_ROUND);
    cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);
    stroke_v_twice (cr, width, height);
}



