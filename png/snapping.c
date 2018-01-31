/*
 * Copyright © 2004 Red Hat, Inc.
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

/* This demo demonstrates how to perform device-pixel snapping for
 * horizontal and vertically aligned strokes and fills. The technique
 * used here is designed to work with any stroke width as well as any
 * scale factor or translation amount in the current transformation.
 * The code here may not provide good results for curved objects or
 * when the transformation includes rotation and or shear components.
 *
 * The output includes four groups of nested boxes. The top two groups
 * consists of 5 filled boxes, alternating between black and
 * white. The bottom two groups show 5 white stroked boxes. Within
 * each group, the path for each box is constructed the same way, but
 * with a different transform. For the two groups on the right, all
 * coordinates in the path are snapped before drawing so that the
 * boundary of each shape will align properly with the device pixel
 * grid.
 */

#include <cairo.h>
#include <math.h>

#define WIDTH 175
#define HEIGHT 175
#define STRIDE (WIDTH * 4)

unsigned char image[STRIDE*HEIGHT];

/* These snapping functions are designed to work properly with a
 * matrix that has only scale and translate components. I make no
 * guarantees about how they will behave under more interesting
 * transformations (such as rotation or shear). */

/* Snap the given coordinate so that it is on an integer coordinate of
 * the device pixel grid. This is the appropriate snapping to use for
 * horizontal/vertical portions of paths to be filled. */
static void
snap_point_for_fill (cairo_t *cr, double *x, double *y)
{
    /* Convert to device space, round, then convert back to user space. */
    cairo_user_to_device (cr, x, y);
    *x = floor (*x + 0.5);
    *y = floor (*y + 0.5);
    cairo_device_to_user (cr, x, y);
}

/* Snap the given path coordinate as appropriate for a path to be
 * stroked. This snapping is dependent on the current line width, so
 * it should be called when the line width is set to the value that
 * will be used for the stroke.
 *
 * The snapping is performed so that the stroke boundary of horizontal
 * and vertical portions will lie precisely between device pixels. If
 * the device-space line width is not an integer, then only one side
 * of the path will be properly aligned. The snap_line_width function
 * below can be used to constrain the line width to be an integer in
 * device space.
 */
static void
snap_point_for_stroke (cairo_t *cr, double *x, double *y)
{
    double x_width_dev_2, y_width_dev_2;
    double x_offset, y_offset;

    /*
     * Round in device space after adding the fractional portion of
     * one-half the (device space) line width.
     */
    x_width_dev_2 = y_width_dev_2 = cairo_get_line_width (cr);
    cairo_user_to_device_distance (cr, &x_width_dev_2, &y_width_dev_2);
    x_width_dev_2 *= 0.5;
    y_width_dev_2 *= 0.5;

    x_offset = x_width_dev_2 - (int) (x_width_dev_2);
    y_offset = y_width_dev_2 - (int) (y_width_dev_2);

    cairo_user_to_device (cr, x, y);
    *x = floor (*x + x_offset + 0.5);
    *y = floor (*y + y_offset + 0.5);
    *x -= x_offset;
    *y -= y_offset;
    cairo_device_to_user (cr, x, y);
}

/* Snap the line width so that it is an integer number of device
 * pixels. Cairo currently only supports symmetrical pens, so if the
 * current transformation has non-uniform scaling in X and Y, we won't
 * be able to satisfy the constraint in both dimensions. So, this
 * function examines both directions and snaps to the dimension that
 * has the larger error. */
static void
snap_line_width (cairo_t *cr)
{
    double x_width, y_width;
    double x_width_snapped, y_width_snapped;
    double x_error, y_error;

    x_width = y_width = cairo_get_line_width (cr);

    cairo_user_to_device_distance (cr, &x_width, &y_width);

    /* If the line width is less than 1 then it will round to 0 and
     * disappear. Instead, we clamp it to 1.0, but we must preserve
     * its sign for the case of a reflecting transformation. */
    x_width_snapped = floor (x_width + 0.5);
    if (fabs(x_width_snapped) < 1.0) {
	if (x_width > 0)
	    x_width_snapped = 1.0;
	else
	    x_width_snapped = -1.0;
    }

    y_width_snapped = floor (y_width + 0.5);
    if (fabs(y_width_snapped) < 1.0) {
	if (y_width > 0)
	    y_width_snapped = 1.0;
	else
	    y_width_snapped = -1.0;
    }

    x_error = fabs (x_width - x_width_snapped);
    y_error = fabs (y_width - y_width_snapped);

    cairo_device_to_user_distance (cr, &x_width_snapped, &y_width_snapped);

    if (x_error > y_error)
	cairo_set_line_width (cr, x_width_snapped);
    else
	cairo_set_line_width (cr, y_width_snapped);
}


typedef struct {
    int first;
    int fill;
    cairo_t *cr;
} snap_path_closure_t;

static void
snap_point (snap_path_closure_t *spc, double *x, double *y)
{
    if (spc->fill)
	snap_point_for_fill (spc->cr, x, y);
    else
	snap_point_for_stroke (spc->cr, x, y);
}

static void
spc_new_path_perhaps (snap_path_closure_t *spc)
{
    if (spc->first) {
	cairo_new_path (spc->cr);
	spc->first = 0;
    }
}

static void
spc_move_to (void *closure, double x, double y)
{
    snap_path_closure_t *spc = closure;
    cairo_t *cr = spc->cr;
    spc_new_path_perhaps (spc);
    snap_point (spc, &x, &y);
    cairo_move_to (cr, x, y);
}

static void
spc_line_to (void *closure, double x, double y)
{
    snap_path_closure_t *spc = closure;
    cairo_t *cr = spc->cr;
    spc_new_path_perhaps (spc);
    snap_point (spc, &x, &y);
    cairo_line_to (cr, x, y);
}

static void
spc_curve_to (void *closure,
	      double x1, double y1_,
	      double x2, double y2,
	      double x3, double y3)
{
    snap_path_closure_t *spc = closure;
    cairo_t *cr = spc->cr;
    spc_new_path_perhaps (spc);
    snap_point (spc, &x1, &y1_);
    snap_point (spc, &x2, &y2);
    snap_point (spc, &x3, &y3);
    cairo_curve_to (cr, x1, y1_, x2, y2, x3, y3);
}

static void
spc_close_path (void *closure)
{
    snap_path_closure_t *spc = closure;
    spc_new_path_perhaps (spc);
    cairo_close_path (spc->cr);
}

static void
snap_path_for_fill (cairo_t *cr)
{
    int i;
    cairo_path_t *path;
    cairo_path_data_t *data;
  
    snap_path_closure_t spc;

    spc.first = 1;
    spc.fill = 1;
    spc.cr = cr;

    path = cairo_copy_path (cr);
  
    for (i=0; i < path->num_data; i += path->data[i].header.length) {
	data = &path->data[i];
	switch (data->header.type) {
	case CAIRO_PATH_MOVE_TO:
	    spc_move_to (&spc, data[1].point.x, data[1].point.y);
	    break;
	case CAIRO_PATH_LINE_TO:
	    spc_line_to (&spc, data[1].point.x, data[1].point.y);
	    break;
	case CAIRO_PATH_CURVE_TO:
	    spc_curve_to (&spc, data[1].point.x, data[1].point.y,
			  data[2].point.x, data[2].point.y,
			  data[3].point.x, data[3].point.y);
	    break;
	case CAIRO_PATH_CLOSE_PATH:
	    spc_close_path (&spc);
	    break;
	}
    }
  
    cairo_path_destroy (path);
}

static void
snap_path_for_stroke (cairo_t *cr)
{
    int i;
    cairo_path_t *path;
    cairo_path_data_t *data;

    snap_path_closure_t spc;

    spc.first = 1;
    spc.fill = 0;
    spc.cr = cr;

    snap_line_width (cr);

    path = cairo_copy_path (cr);
  
    for (i=0; i < path->num_data; i += path->data[i].header.length) {
	data = &path->data[i];
	switch (data->header.type) {
	case CAIRO_PATH_MOVE_TO:
	    spc_move_to (&spc, data[1].point.x, data[1].point.y);
	    break;
	case CAIRO_PATH_LINE_TO:
	    spc_line_to (&spc, data[1].point.x, data[1].point.y);
	    break;
	case CAIRO_PATH_CURVE_TO:
	    spc_curve_to (&spc, data[1].point.x, data[1].point.y,
			  data[2].point.x, data[2].point.y,
			  data[3].point.x, data[3].point.y);
	    break;
	case CAIRO_PATH_CLOSE_PATH:
	    spc_close_path (&spc);
	    break;
	}
    }
  
    cairo_path_destroy (path);
}

typedef enum { NESTED_FILLS, NESTED_STROKES } nested_style_t;
typedef enum { NO_SNAPPING, SNAPPING } snapping_t;

static void
draw_nested (cairo_t *cr, nested_style_t style, snapping_t snapping)
{
#define NUM_BOXES 5
#define BOX_WIDTH 13
/* We need non-integer scale factors to demonstrate anything interesting. */
#define SCALE_TWEAK 1.11

    int i;
    double scale, offset = SCALE_TWEAK * BOX_WIDTH / 2.0;

    cairo_save (cr);

    cairo_set_line_width (cr, 1.0);

    for (i=0; i < NUM_BOXES; i++) {
	scale = SCALE_TWEAK * (NUM_BOXES - i);

	cairo_save (cr);
	{
	    cairo_scale (cr, scale, scale);

	    cairo_rectangle (cr, 0, 0, BOX_WIDTH, BOX_WIDTH);

	    if (style == NESTED_FILLS) {
		if (snapping == SNAPPING)
		    snap_path_for_fill (cr);
		if (i % 2 == 0)
		    cairo_set_source_rgb (cr, 1, 1, 1);
		else
		    cairo_set_source_rgb (cr, 0, 0, 0);
		cairo_fill (cr);
	    } else {
		if (snapping == SNAPPING)
		    snap_path_for_stroke (cr);
		cairo_set_source_rgb (cr, 1, 1, 1);
		cairo_stroke (cr);
	    }
	}
	cairo_restore (cr);

	cairo_translate (cr, offset, offset);
    }

    cairo_restore (cr);
}

static void
draw (cairo_t *cr, int width, int height)
{
    cairo_translate (cr, 6, 6);

    draw_nested (cr, NESTED_FILLS, NO_SNAPPING);

    cairo_translate (cr, width / 2, 0);

    draw_nested (cr, NESTED_FILLS, SNAPPING);

    cairo_translate (cr, -width / 2, height / 2);

    draw_nested (cr, NESTED_STROKES, NO_SNAPPING);

    cairo_translate (cr, width / 2, 0);

    draw_nested (cr, NESTED_STROKES, SNAPPING);
}

int
main (void)
{
    cairo_surface_t *surface;
    cairo_t *cr;

    surface = cairo_image_surface_create_for_data (image, CAIRO_FORMAT_ARGB32,
						   WIDTH, HEIGHT, STRIDE);

    cr = cairo_create (surface);

    cairo_rectangle (cr, 0, 0, WIDTH, HEIGHT);
    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_fill (cr);

    draw (cr, WIDTH, HEIGHT);

    cairo_surface_write_to_png (surface, "snapping.png");

    cairo_destroy (cr);

    cairo_surface_destroy (surface);

    return 0;
}

