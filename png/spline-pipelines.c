/* cc `pkg-config --cflags --libs cairo libpng12` spline-pipelines.c -o spline-pipelines
 *
 * Copyright © 2006 Red Hat, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of
 * Red Hat, Inc. not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission. Red Hat, Inc. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * RED HAT, INC. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL RED HAT, INC. BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Carl D. Worth <cworth@cworth.org>
 */

/* Here's a simple demo of how separate transformations can affect the
 * pen for stroking compared to the path itself.
 *
 * This was inspired by an image by Maxim Shemanarev demonstrating the
 * flexible-pipeline nature of his Antigrain Geometry project:
 *
 *	http://antigrain.com/tips/line_alignment/conv_order.gif
 */

#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>

static void
spline_path (cairo_t *cr)
{
    cairo_save (cr);
    {
	cairo_translate (cr, -106.0988385, -235.84433);
	cairo_move_to (cr,
		       49.517857, 235.84433);
	cairo_curve_to (cr,
			86.544809, 175.18401,
			130.19603, 301.40165,
			162.67982, 240.42946);
    }
    cairo_restore (cr);
}

#define LINE_WIDTH 13

static void
source_path (cairo_t *cr)
{
    spline_path (cr);
    cairo_set_line_width (cr, 1.0);
    cairo_stroke (cr);
}

static void
stroke (cairo_t *cr)
{
    spline_path (cr);
    cairo_set_line_width (cr, LINE_WIDTH);
    cairo_stroke (cr);
}

static void
scale_both_set_line_width_stroke (cairo_t *cr)
{
    cairo_scale (cr, 0.5, 0.5);
    spline_path (cr);
    cairo_set_line_width (cr, LINE_WIDTH);
    cairo_stroke (cr);
}

static void
scale_both_set_line_width_double_stroke (cairo_t *cr)
{
    cairo_scale (cr, 0.5, 0.5);
    spline_path (cr);
    cairo_set_line_width (cr, 2 * LINE_WIDTH);
    cairo_stroke (cr);
}

static void
save_scale_path_restore_set_line_width_stroke (cairo_t *cr)
{
    cairo_save (cr);
    {
	cairo_scale (cr, 0.5, 1.0);
	spline_path (cr);
    }
    cairo_restore (cr);

    cairo_set_line_width (cr, LINE_WIDTH);
    cairo_stroke (cr);
}

/* XXX: Ouch. It looks like there's an API bug in the implemented
 * semantics for cairo_set_line_width. I believe the following
 * function (set_line_width_scale_path_stroke_BUGGY) should result in
 * a figure identical to the version above it
 * (save_scale_path_restore_set_line_width_stroke), but it's currently
 * giving the same result as the one beloe
 * (scale_path_set_line_width_stroke). */
static void
set_line_width_scale_path_stroke_BUGGY (cairo_t *cr)
{
    cairo_set_line_width (cr, LINE_WIDTH);
    cairo_scale (cr, 0.5, 1.0);
    spline_path (cr);
    cairo_stroke (cr);
}

static void
scale_path_set_line_width_stroke (cairo_t *cr)
{
    cairo_scale (cr, 0.5, 1.0);
    cairo_set_line_width (cr, LINE_WIDTH);
    spline_path (cr);
    cairo_stroke (cr);
}

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))

int
main (void)
{
    cairo_surface_t *surface;
    cairo_t *cr;
    int i;
    typedef void (*pipeline_t) (cairo_t *cr);
    pipeline_t pipelines[] = {
	source_path,
	stroke,
	scale_both_set_line_width_stroke,
	scale_both_set_line_width_double_stroke,
	save_scale_path_restore_set_line_width_stroke,
	scale_path_set_line_width_stroke
    };
    int width = 140;
    int height = 68.833 * ARRAY_SIZE(pipelines);

    surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);
    cr = cairo_create (surface);

    for (i = 0; i < ARRAY_SIZE(pipelines); i++) {
	cairo_save (cr);
	cairo_translate (cr, width/2,  (i+0.5) * (height / (double) ARRAY_SIZE(pipelines)));
	(pipelines[i]) (cr);
	cairo_restore (cr);
	if (cairo_status (cr)) {
	    fprintf (stderr, "Cairo is unhappy after pipeline #%d: %s\n",
		     i, cairo_status_to_string (cairo_status (cr)));
	    exit (1);
	}
    }

    cairo_surface_write_to_png (surface, "spline-pipelines.png");

    cairo_destroy (cr);
    cairo_surface_destroy (surface);

    return 0;
}
