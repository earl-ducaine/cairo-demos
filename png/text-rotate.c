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

#define TEXT "hello, world"

#define WIDTH 450
#define HEIGHT 900
#define STRIDE (WIDTH * 4)

unsigned char image[STRIDE*HEIGHT];
int
main (void)
{
    int i;
    cairo_surface_t *surface;
    cairo_t *cr;
    cairo_matrix_t matrix;
    double angle;

    surface = cairo_image_surface_create_for_data (image, CAIRO_FORMAT_ARGB32,
						   WIDTH, HEIGHT, STRIDE);

    cr = cairo_create (surface);

    cairo_set_source_rgb (cr, 0., 0., 0.);

    cairo_select_font_face (cr, "serif", 0, 0);
    cairo_set_font_size (cr, 40);
    cairo_translate (cr, 40, 40);

#define NUM_STRINGS 3.0
    cairo_select_font_face (cr, "mono", 0, 0);

    cairo_set_font_size (cr, 12);
    cairo_show_text (cr, "+CTM rotation");

    cairo_save (cr);
    cairo_select_font_face (cr, "serif", 0, 0);
    cairo_set_font_size (cr, 40);
    for (i=0; i < NUM_STRINGS; i++) {
	angle = (double) i * 0.5 * M_PI / (NUM_STRINGS - 1);
	cairo_save (cr);
	cairo_rotate (cr, angle);
	cairo_move_to (cr, 100, 0);
	cairo_show_text (cr, "Text");
	cairo_restore (cr);
    }
    cairo_restore (cr);

    cairo_translate (cr, 0, HEIGHT / 3);

    cairo_move_to (cr, 0, 0);
    cairo_show_text (cr, "+CTM rotation");
    cairo_rel_move_to (cr, 0, 12);
    cairo_show_text (cr, "-font rotation");

    cairo_save (cr);
    cairo_select_font_face (cr, "serif", 0, 0);
    cairo_set_font_size (cr, 40);
    for (i=0; i < NUM_STRINGS; i++) {
	angle = (double) i * 0.5 * M_PI / (NUM_STRINGS - 1);
	cairo_save (cr);
	cairo_rotate (cr, angle);
	cairo_matrix_init_identity (&matrix);
	cairo_matrix_scale (&matrix, 40, 40);
	cairo_matrix_rotate (&matrix, - angle);
	cairo_set_font_matrix (cr, &matrix);
	cairo_move_to (cr, 100, 0);
	cairo_show_text (cr, "Text");
	cairo_restore (cr);
    }
    cairo_restore (cr);

    cairo_translate (cr, 0, HEIGHT / 3);

    cairo_move_to (cr, 0, 0);
    cairo_show_text (cr, "+CTM rotation");
    cairo_rel_move_to (cr, 0, 12);
    cairo_show_text (cr, "-CTM rotation");

    cairo_save (cr);
    cairo_select_font_face (cr, "serif", 0, 0);
    cairo_set_font_size (cr, 40);
    for (i=0; i < NUM_STRINGS; i++) {
	angle = (double) i * 0.5 * M_PI / (NUM_STRINGS - 1);
	cairo_save (cr);
	cairo_rotate (cr, angle);
	cairo_move_to (cr, 100, 0);
	cairo_rotate (cr, -angle);
	cairo_show_text (cr, "Text");
	cairo_restore (cr);
    }
    cairo_restore (cr);

    cairo_surface_write_to_png (surface, "text-rotate.png");

    cairo_destroy (cr);

    cairo_surface_destroy (surface);

    return 0;
}
