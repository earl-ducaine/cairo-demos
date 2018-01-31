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
#define HEIGHT 600
#define STRIDE (WIDTH * 4)

#define DOUBLE_TO_26_6(d) ((FT_F26Dot6)((d) * 64.0))
#define DOUBLE_FROM_26_6(t) ((double)(t) / 64.0)
#define DOUBLE_TO_16_16(d) ((FT_Fixed)((d) * 65536.0))
#define DOUBLE_FROM_16_16(t) ((double)(t) / 65536.0)

unsigned char image[STRIDE*HEIGHT];

static void
box_text (cairo_t *cr, const char *utf8, double x, double y)
{
    double line_width;
    cairo_text_extents_t extents;

    cairo_save (cr);

    cairo_text_extents (cr, TEXT, &extents);
    line_width = cairo_get_line_width (cr);
    cairo_rectangle (cr,
		     x + extents.x_bearing - line_width,
		     y + extents.y_bearing - line_width,
		     extents.width  + 2 * line_width,
		     extents.height + 2 * line_width);
    cairo_stroke (cr);

    cairo_move_to (cr, x, y);
    cairo_show_text (cr, utf8);
    cairo_move_to (cr, x, y);
    cairo_text_path (cr, utf8);
    cairo_set_source_rgb (cr, 1, 0, 0);
    cairo_set_line_width (cr, 1.0);
    cairo_stroke (cr);

    cairo_restore (cr);
}

static void
box_glyphs (cairo_t *cr, cairo_glyph_t *glyphs, int num_glyphs,
	    double x, double y)
{
    int i;
    double line_width;
    cairo_text_extents_t extents;

    cairo_save (cr);

    cairo_glyph_extents (cr, glyphs, num_glyphs, &extents);
    line_width = cairo_get_line_width (cr);
    cairo_rectangle (cr,
		     x + extents.x_bearing - line_width,
		     y + extents.y_bearing - line_width,
		     extents.width  + 2 * line_width,
		     extents.height + 2 * line_width);
    cairo_stroke (cr);

    for (i=0; i < num_glyphs; i++) {
	glyphs[i].x += x;
	glyphs[i].y += y;
    }
    cairo_show_glyphs (cr, glyphs, num_glyphs);
    cairo_glyph_path (cr, glyphs, num_glyphs);
    cairo_set_source_rgb (cr, 1, 0, 0);
    cairo_set_line_width (cr, 1.0);
    cairo_stroke (cr);
    for (i=0; i < num_glyphs; i++) {
	glyphs[i].x -= x;
	glyphs[i].y -= y;
    }

    cairo_restore (cr);
}

int
main (void)
{
    int i;
    cairo_surface_t *surface;
    cairo_t *cr;
    cairo_text_extents_t extents;
    cairo_font_extents_t font_extents;
    double dx, dy;
    double height;
#define NUM_GLYPHS 10
    cairo_glyph_t glyphs[NUM_GLYPHS];

    surface = cairo_image_surface_create_for_data (image, CAIRO_FORMAT_ARGB32,
						   WIDTH, HEIGHT, STRIDE);


    cr = cairo_create (surface);

    cairo_set_source_rgb (cr, 0., 0., 0.);
    cairo_set_line_width (cr, 2.0);

    cairo_save (cr);
    cairo_rectangle (cr, 0., 0., WIDTH, HEIGHT);
    cairo_set_source_rgba (cr, 0., 0., 0., 0.);
    cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
    cairo_fill (cr);
    cairo_restore (cr);

    cairo_select_font_face (cr, "sans", 0, 0);
    cairo_set_font_size (cr, 40);
#define XXX_DEMONSTRATE_EXTENTS_BUGS_WHEN_FONT_IS_TRANSFORMED 1
#if XXX_DEMONSTRATE_EXTENTS_BUGS_WHEN_FONT_IS_TRANSFORMED
    {
	cairo_matrix_t matrix;
	cairo_matrix_init_scale (&matrix, 40.0, -40.0);
	cairo_set_font_matrix (cr, &matrix);
    }

    cairo_scale (cr, 1.0, -1.0);
    cairo_translate (cr, 0.0, - HEIGHT);
#endif

    cairo_font_extents (cr, &font_extents);
    height = font_extents.height;

    dx = 0.0;
    dy = 0.0;
    for (i=0; i < NUM_GLYPHS; i++) {
	glyphs[i].index = i + 4;
	glyphs[i].x = dx;
	glyphs[i].y = dy;
	cairo_glyph_extents (cr, &glyphs[i], 1, &extents);
	dx += extents.x_advance;
	dy += extents.y_advance;
    }

    box_text (cr, TEXT, 10, height);

    cairo_translate (cr, 0, height);
    cairo_save (cr);
    {
	cairo_translate (cr, 10, height);
	cairo_rotate (cr, 10 * M_PI / 180.0);
	box_text (cr, TEXT, 0, 0);
    }
    cairo_restore (cr);

    cairo_translate (cr, 0, 2 * height);
    cairo_save (cr);
    {
	cairo_matrix_t matrix;
	cairo_matrix_init_identity (&matrix);
	cairo_matrix_scale (&matrix, 40, -40);
	cairo_matrix_rotate (&matrix, -10 * M_PI / 180.0);
	cairo_set_font_matrix (cr, &matrix);
    }
    box_text (cr, TEXT, 10, height);
    cairo_restore (cr);

    cairo_translate (cr, 0, 2 * height);
    box_glyphs (cr, glyphs, NUM_GLYPHS, 10, height);

    cairo_translate (cr, 10, 2 * height);
    cairo_save (cr);
    {
	cairo_rotate (cr, 10 * M_PI / 180.0);
	box_glyphs (cr, glyphs, NUM_GLYPHS, 0, 0);
    }
    cairo_restore (cr);

    cairo_translate (cr, 0, height);
    for (i=0; i < NUM_GLYPHS; i++)
	glyphs[i].y += i * 5;
    box_glyphs (cr, glyphs, NUM_GLYPHS, 10, height);

    cairo_surface_write_to_png (surface, "text.png");

    cairo_destroy (cr);

    cairo_surface_destroy (surface);

    return 0;
}
