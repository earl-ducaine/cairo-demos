/*
 * $Id: fdhand.c,v 1.2 2005-05-11 22:40:18 tor Exp $
 *
 * Copyright © 2003 Keith Packard
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Keith Packard not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Keith Packard makes no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * KEITH PACKARD DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL KEITH PACKARD BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include <cairo.h>

#include "fdhand.h"
#include <sys/time.h>
#include <time.h>
#include <math.h>

#define PI_6	(M_PI/6.0)

static void
draw_hour (cairo_t *cr, double width, double length)
{
    double  r = width / 2;
    cairo_move_to (cr,	length,		    -r);
    cairo_arc     (cr,	length,		    0,		r, -M_PI_2, M_PI_2);
    cairo_line_to (cr,  width * M_SQRT2,    r);
    cairo_arc     (cr,	0,		    0,		r*2, PI_6, - PI_6);
    cairo_close_path (cr);
}

static void
draw_minute (cairo_t *cr, double width, double length)
{
    double  r = width / 2;
    cairo_move_to (cr,	length,		    -r);
    cairo_arc     (cr,	length,		    0,		r, -M_PI_2, M_PI_2);
    cairo_line_to (cr,  0,		    r);
    cairo_line_to (cr,	0,		    -r);
    cairo_close_path (cr);
}

static void
draw_second (cairo_t *cr, double width, double length)
{
    double  r = width / 2;
    double  thick = width;
    double  back = length / 3;
    double  back_thin = length / 10;

    cairo_move_to (cr,	length,		    -r);
    cairo_arc     (cr,	length,		    0,		r, -M_PI_2, M_PI_2);
    cairo_line_to (cr,  -back_thin,    	    r);
    cairo_line_to (cr,  -back_thin,	    thick);
    cairo_line_to (cr,	-back,		    thick);
    cairo_line_to (cr,	-back,		    -thick);
    cairo_line_to (cr,	-back_thin,	    -thick);
    cairo_line_to (cr,	-back_thin,	    -r);
    cairo_close_path (cr);
}

static void
draw_hand (cairo_t *cr, double noon_deg, double width, double length, double alt,
	   void (*draw) (cairo_t *cr, double width, double length))
{
    cairo_matrix_t m;
    cairo_get_matrix (cr, &m);
    {
	cairo_translate (cr, alt/2, alt);
	cairo_rotate (cr, noon_deg * M_PI / 180.0 - M_PI_2);
	(*draw) (cr, width, length);
    }
    cairo_set_matrix (cr, &m);
}

#define HOUR_WIDTH	0.03
#define HOUR_LENGTH	0.25
#define HOUR_ALT	0.010

#define MINUTE_WIDTH	0.015
#define MINUTE_LENGTH	0.39
#define MINUTE_ALT	0.020

#define SECOND_WIDTH	0.0075
#define SECOND_LENGTH	0.32
#define SECOND_ALT	0.026

static void
draw_time (cairo_t *cr, double width, double height, struct timeval *tv, int seconds)
{
    double  hour_angle, minute_angle, second_angle;
    struct tm	tm_ret;
    struct tm	*tm;
    tm = localtime_r (&tv->tv_sec, &tm_ret);
    
    second_angle = (tm->tm_sec + tv->tv_usec / 1000000.0) * 6.0;
    minute_angle = tm->tm_min * 6.0 + second_angle / 60.0;
    hour_angle = tm->tm_hour * 30.0 + minute_angle / 12.0;

    cairo_save (cr);
    {
	cairo_scale (cr, width, height);
	cairo_translate (cr, 0.5, 0.5);
	draw_hand (cr, hour_angle, HOUR_WIDTH, HOUR_LENGTH, 
		   HOUR_ALT, draw_hour);
	draw_hand (cr, minute_angle, MINUTE_WIDTH, MINUTE_LENGTH,
		   MINUTE_ALT, draw_minute);
	if (seconds)
	    draw_hand (cr, second_angle, SECOND_WIDTH, SECOND_LENGTH,
		       SECOND_ALT, draw_second);
	cairo_set_source_rgba (cr, 0, 0, 0, 0.3);
	cairo_fill (cr);
	
	cairo_set_source_rgba (cr, 0, 0, 0, 1);
	draw_hand (cr, hour_angle, HOUR_WIDTH, HOUR_LENGTH, 
		   0.0, draw_hour);
	cairo_fill (cr);
	draw_hand (cr, minute_angle, MINUTE_WIDTH, MINUTE_LENGTH,
		   0.0, draw_minute);
	cairo_fill (cr);
	if (seconds)
	    draw_hand (cr, second_angle, SECOND_WIDTH, SECOND_LENGTH,
		       0.0, draw_second);
	cairo_fill (cr);
    }
    cairo_restore (cr);
}

void
fdhand_draw_now (cairo_t *cr, double width, double height, int seconds)
{
    struct timeval  tv;
    struct timezone tz;
    gettimeofday (&tv, &tz);

    draw_time (cr, width, height, &tv, seconds);
}

