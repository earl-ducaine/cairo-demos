/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/* Small example demonstrating emulating knockout-groups as in PDF-1.4
 * using cairo_set_operator().
 *
 * Owen Taylor,

 * v0.1  30 November  2002
 * v0.2   1 December  2002 - typo fixes from Keith Packard
 * v0.3  17 April     2003 - Tracking changes in Xr, (Removal of Xr{Push,Pop}Group)
 * v0.4  29 September 2003 - Use cairo_rectangle rather than private rect_path
 *			     Use cairo_arc for oval_path
 * Keeping log of changes in ChangeLog/CVS now. (2003-11-19) Carl Worth
 */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <cairo.h>
#include <cairo-xlib.h>
#include <math.h>
#include <stdio.h>

/* Create a path that is a circular oval with radii xr, yr at xc,
 * yc. Since we use cairo_arc here it will provide as many splines as
 * necessary to be extremely accurate. 
 */
static void
oval_path (cairo_t *cr,
           double xc, double yc,
           double xr, double yr)
{
    cairo_matrix_t matrix;

    cairo_get_matrix (cr, &matrix);

    cairo_translate (cr, xc, yc);
    cairo_scale (cr, 1.0, yr / xr);
    cairo_move_to (cr, xr, 0.0);
    cairo_arc (cr,
               0, 0,
               xr,
               0, 2 * M_PI);
    cairo_close_path (cr);

    cairo_set_matrix (cr, &matrix);
}

/* Fill the given area with checks in the standard style
 * for showing compositing effects.
 */
static void
fill_checks (cairo_t *cr,
             int x,     int y,
             int width, int height)
{
    cairo_surface_t *check;
    cairo_pattern_t *check_pattern;
    
    cairo_save (cr);

#define CHECK_SIZE 32

    check = cairo_surface_create_similar (cairo_get_target (cr),
                                          CAIRO_CONTENT_COLOR,
                                          2 * CHECK_SIZE, 2 * CHECK_SIZE);

    /* Draw the check */
    {
        cairo_t *cr2;

        cr2 = cairo_create (check);

        cairo_set_operator (cr2, CAIRO_OPERATOR_SOURCE);

        cairo_set_source_rgb (cr2, 0.4, 0.4, 0.4);

        cairo_rectangle (cr2, 0, 0, 2 * CHECK_SIZE, 2 * CHECK_SIZE);
        cairo_fill (cr2);

        cairo_set_source_rgb (cr2, 0.7, 0.7, 0.7);

        cairo_rectangle (cr2, x, y, CHECK_SIZE, CHECK_SIZE);
        cairo_fill (cr2);
        cairo_rectangle (cr2, x + CHECK_SIZE, y + CHECK_SIZE, CHECK_SIZE, CHECK_SIZE);
        cairo_fill (cr2);

        cairo_destroy (cr2);
    }

    /* Fill the whole surface with the check */

    check_pattern = cairo_pattern_create_for_surface (check);
    cairo_pattern_set_extend (check_pattern, CAIRO_EXTEND_REPEAT);
    cairo_set_source (cr, check_pattern);
    cairo_rectangle (cr, 0, 0, width, height);
    cairo_fill (cr);

    cairo_pattern_destroy (check_pattern);
    cairo_surface_destroy (check);

    cairo_restore (cr);
}

/* Draw a red, green, and blue circle equally spaced inside
 * the larger circle of radius r at (xc, yc)
 */
static void
draw_3circles (cairo_t *cr,
               double xc, double yc,
               double radius,
               double alpha)
{
    double subradius = radius * (2 / 3. - 0.1);
    
    cairo_set_source_rgba (cr, 1., 0., 0., alpha);
    oval_path (cr,
               xc + radius / 3. * cos (M_PI * (0.5)),
               yc - radius / 3. * sin (M_PI * (0.5)),
               subradius, subradius);
    cairo_fill (cr);
    
    cairo_set_source_rgba (cr, 0., 1., 0., alpha);
    oval_path (cr,
               xc + radius / 3. * cos (M_PI * (0.5 + 2/.3)),
               yc - radius / 3. * sin (M_PI * (0.5 + 2/.3)),
               subradius, subradius);
    cairo_fill (cr);
    
    cairo_set_source_rgba (cr, 0., 0., 1., alpha);
    oval_path (cr,
               xc + radius / 3. * cos (M_PI * (0.5 + 4/.3)),
               yc - radius / 3. * sin (M_PI * (0.5 + 4/.3)),
               subradius, subradius);
    cairo_fill (cr);
}

static void
draw (cairo_t *cr,
      int      width,
      int      height)
{
    cairo_surface_t *overlay, *punch, *circles;

    /* Fill the background */
    double radius = 0.5 * (width < height ? width : height) - 10;
    double xc = width / 2.;
    double yc = height / 2.;

    overlay = cairo_surface_create_similar (cairo_get_target (cr),
                                            CAIRO_CONTENT_COLOR_ALPHA,
                                            width, height);
    if (overlay == NULL)
        return;

    punch = cairo_surface_create_similar (cairo_get_target (cr),
                                          CAIRO_CONTENT_ALPHA,
                                          width, height);
    if (punch == NULL)
        return;

    circles = cairo_surface_create_similar (cairo_get_target (cr),
                                            CAIRO_CONTENT_COLOR_ALPHA,
                                            width, height);
    if (circles == NULL)
        return;
    
    fill_checks (cr, 0, 0, width, height);

    cairo_save (cr);

    {
        cairo_t *cr_overlay;

        cr_overlay = cairo_create (overlay);

        /* Draw a black circle on the overlay
         */
        cairo_set_source_rgb (cr_overlay, 0., 0., 0.);
        oval_path (cr_overlay, xc, yc, radius, radius);
        cairo_fill (cr_overlay);

        {
            cairo_t *cr_tmp;

            cr_tmp = cairo_create (punch);

            /* Draw 3 circles to the punch surface, then cut
             * that out of the main circle in the overlay
             */
            draw_3circles (cr_tmp, xc, yc, radius, 1.0);

            cairo_destroy (cr_tmp);
        }

        cairo_set_operator (cr_overlay, CAIRO_OPERATOR_DEST_OUT);
        cairo_set_source_surface (cr_overlay, punch, 0, 0);
        cairo_paint (cr_overlay);

        /* Now draw the 3 circles in a subgroup again
         * at half intensity, and use OperatorAdd to join up
         * without seams.
         */
        {
            cairo_t *cr_tmp;

            cr_tmp = cairo_create (circles);

            cairo_set_operator (cr_tmp, CAIRO_OPERATOR_OVER);
            draw_3circles (cr_tmp, xc, yc, radius, 0.5);
            
            cairo_destroy (cr_tmp);
        }

        cairo_set_operator (cr_overlay, CAIRO_OPERATOR_ADD);
        cairo_set_source_surface (cr_overlay, circles, 0, 0);
        cairo_paint (cr_overlay);

        cairo_destroy (cr_overlay);
    }

    cairo_set_source_surface (cr, overlay, 0, 0);
    cairo_paint (cr);

    cairo_surface_destroy (overlay);
    cairo_surface_destroy (punch);
    cairo_surface_destroy (circles);

}

int
main (int argc, char **argv)
{
  Display *dpy;
  int screen;
  Window w;
  Pixmap pixmap;
  char *title = "cairo: Knockout Groups";
  unsigned int quit_keycode;
  int needs_redraw;
  GC gc;
  XWMHints *wmhints;
  XSizeHints *normalhints;
  XClassHint *classhint;
  
  int width = 400;
  int height = 400;
  
  dpy = XOpenDisplay (NULL);
  screen = DefaultScreen (dpy);

  w = XCreateSimpleWindow (dpy, RootWindow (dpy, screen),
			   0, 0, width, height, 0,
			   BlackPixel (dpy, screen), WhitePixel (dpy, screen));

  normalhints = XAllocSizeHints ();
  normalhints->flags = 0;
  normalhints->x = 0;
  normalhints->y = 0;
  normalhints->width = width;
  normalhints->height = height;

  classhint = XAllocClassHint ();
  classhint->res_name = "cairo-knockout";
  classhint->res_class = "Cairo-knockout";
    
  wmhints = XAllocWMHints ();
  wmhints->flags = InputHint;
  wmhints->input = True;
    
  XmbSetWMProperties (dpy, w, title, "cairo-knockout", 0, 0, 
                      normalhints, wmhints, classhint);
  XFree (wmhints);
  XFree (classhint);
  XFree (normalhints);

  pixmap = XCreatePixmap (dpy, w, width, height, DefaultDepth (dpy, screen));
  gc = XCreateGC (dpy, pixmap, 0, NULL);

  quit_keycode = XKeysymToKeycode(dpy, XStringToKeysym("Q"));

  XSelectInput (dpy, w, ExposureMask | StructureNotifyMask | ButtonPressMask | KeyPressMask);
  XMapWindow (dpy, w);
  
  needs_redraw = 1;

  while (1) {
      XEvent xev;

      /* Only do the redraw if there are no events pending.  This
       * avoids us getting behind doing several redraws for several
       * consecutive resize events for example.
       */
      if (!XPending (dpy) && needs_redraw) {
          cairo_surface_t *surface;
          cairo_t *cr;
          surface = cairo_xlib_surface_create (dpy, pixmap,
                                               DefaultVisual (dpy, DefaultScreen (dpy)),
                                               width, height);
          cr = cairo_create (surface);

          draw (cr, width, height);

          cairo_destroy (cr);
          cairo_surface_destroy (surface);

          XCopyArea (dpy, pixmap, w, gc,
                     0, 0,
                     width, height,
                     0, 0);

          needs_redraw = 0;
      }
      
      XNextEvent (dpy, &xev);

      switch (xev.xany.type) {
      case ButtonPress:
          /* A click on the canvas ends the program */
          goto DONE;
      case KeyPress:
          if (xev.xkey.keycode == quit_keycode)
              goto DONE;
          break;
      case ConfigureNotify:
          /* Note new size and create new pixmap. */
	  width = xev.xconfigure.width;
	  height = xev.xconfigure.height;
          XFreePixmap (dpy, pixmap);
          pixmap = XCreatePixmap (dpy, w, width, height, DefaultDepth (dpy, screen));
          needs_redraw = 1;
	  break;
      case Expose:
          XCopyArea (dpy, pixmap, w, gc,
                     xev.xexpose.x, xev.xexpose.y,
                     xev.xexpose.width, xev.xexpose.height,
                     xev.xexpose.x, xev.xexpose.y);
	  break;
      }
  }
  DONE:

  XFreeGC (dpy, gc);
  XCloseDisplay (dpy);

  return 0;
}
