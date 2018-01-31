#include <gtk/gtk.h>
#include <cairo.h>
#include <math.h>
#include <gdk/gdkkeysyms.h>
#include <string.h>

#define WIDTH  800
#define HEIGHT 600

#define STRIDE WIDTH*4

#define MAX_COORDS 1024

/* uncomment this to decrease the density of coordinate samples used for
 * the current stroke, this is not an optimal solution, a better solution
 * would be to use actual smoothing on the coordinate data
 */
#define USE_HINT

GtkWidget *canvas = NULL;

gboolean pen_color_is_black = TRUE;
gint     pen_radius         = 8;

gdouble coord_x [MAX_COORDS];
gdouble coord_y [MAX_COORDS];
gint    coord_count = 0;

guchar buffer      [WIDTH*HEIGHT*4];
guchar temp_buffer [WIDTH*HEIGHT*4];

cairo_surface_t *back_surface = NULL;
cairo_surface_t *temp_surface = NULL;
cairo_t         *cr_save    = NULL;

gboolean pen_is_down = FALSE;

/* utility functions, defined at bottom,
 * constructing a path from coordinate arrays
 */
static void
points_to_linear_path (cairo_t *cr,
                       gdouble  coord_x[],
                       gdouble  coord_y[],
                       gint     n_coords);
static void
points_to_bezier_path (cairo_t *cr,
                       gdouble  coord_x[],
                       gdouble  coord_y[],
                       gint     n_coords);


static void
drawapp_render (cairo_t *cr)
{
  cairo_save (cr);

  cairo_set_source_rgb (cr, 1,1,1);
  cairo_paint (cr);

  cairo_set_source_surface (cr, back_surface, 0, 0);
  cairo_paint (cr);

  cairo_set_line_width (cr, pen_radius*2);

  if (pen_color_is_black)
      cairo_set_source_rgba (cr, 0,0,0, 0.5);
  else
      cairo_set_source_rgba (cr, 1,1,1, 0.5);

  cairo_set_line_join (cr, CAIRO_LINE_JOIN_ROUND);
  cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);

  points_to_bezier_path (cr, coord_x, coord_y, coord_count);
  cairo_stroke (cr);

  cairo_restore (cr);
}

static void
apply_coords (guchar *buffer)
{
  drawapp_render (cr_save);
  memcpy (buffer, temp_buffer, HEIGHT*STRIDE);
}

static void
coords_clear (void)
{
  coord_count = 0;
}

static void
coords_add (gdouble x,
            gdouble y)
{
  if (coord_count< MAX_COORDS-3)
    {
      coord_x [coord_count]=x;
      coord_y [coord_count]=y;
      coord_count++;
    }
}

static void
paint (GtkWidget      *widget,
       GdkEventExpose *eev,
       gpointer        data)
{
  cairo_t *cr;
  cr = gdk_cairo_create (widget->window);
  drawapp_render (cr);
  cairo_destroy (cr);
}

static void
pen_motion (gdouble  x,
            gdouble  y)
{
  if (pen_is_down)
    {
      coords_add (x,y);
      gtk_widget_queue_draw (canvas);
    }
}

static void
pen_down (gdouble x,
          gdouble y)
{
  pen_is_down = TRUE;
  coords_add (x,y);
  gtk_widget_queue_draw (canvas);
}

static void
pen_up (gdouble x,
        gdouble y)
{
  pen_is_down = FALSE;
  apply_coords (buffer);
  coords_clear ();
  gtk_widget_queue_draw (canvas);
}


static void
init (void)
{
  coords_clear ();

  memset (buffer, 0, sizeof(buffer));
  back_surface = cairo_image_surface_create_for_data (buffer,
               CAIRO_FORMAT_ARGB32, WIDTH, HEIGHT, STRIDE);

  temp_surface = cairo_image_surface_create_for_data (temp_buffer,
               CAIRO_FORMAT_ARGB32, WIDTH, HEIGHT, STRIDE);
  memset (temp_buffer, 0, sizeof(temp_buffer));

  cr_save = cairo_create (temp_surface);
}

/* just wrapping the gtk events */

static gboolean
event_press (GtkWidget      *widget,
             GdkEventButton *bev,
             gpointer        user_data)
{
  pen_down (bev->x, bev->y);
  return TRUE;
}

static gboolean
event_release (GtkWidget      *widget,
               GdkEventButton *bev,
               gpointer        user_data)
{
  pen_up (bev->x, bev->y);
  return TRUE;
}

static gboolean
event_motion (GtkWidget      *widget,
              GdkEventMotion *mev,
              gpointer        user_data)
{
  pen_motion (mev->x, mev->y);
  gdk_window_get_pointer (widget->window, NULL, NULL, NULL);

  return TRUE;
}

static gboolean
event_keypress (GtkWidget   *widget,
                GdkEventKey *kev,
                gpointer     user_data)
{
  switch (kev->keyval)
    {
      case GDK_x:
      case GDK_X:
        pen_color_is_black = !pen_color_is_black;
      default:
        break;
    }
  return TRUE;
}

gint
main (gint    argc,
      gchar **argv)
{
  GtkWidget *mainwin;

  gtk_init (&argc, &argv);

  mainwin = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  canvas = gtk_drawing_area_new ();

  gtk_widget_set_events (canvas,
                         GDK_EXPOSURE_MASK            |
#ifdef USE_HINT
                         GDK_POINTER_MOTION_HINT_MASK |
#endif
                         GDK_BUTTON1_MOTION_MASK      |
                         GDK_BUTTON_PRESS_MASK        |
                         GDK_KEY_PRESS_MASK           |
                         GDK_BUTTON_RELEASE_MASK);
  gtk_widget_set_size_request (canvas, WIDTH, HEIGHT);

  g_signal_connect (mainwin, "destroy",
		    G_CALLBACK (gtk_main_quit), NULL);
  g_signal_connect (G_OBJECT (canvas), "expose-event",
                    G_CALLBACK (paint), NULL);
  g_signal_connect (G_OBJECT (canvas), "motion_notify_event",
                    G_CALLBACK (event_motion), NULL);
  g_signal_connect (G_OBJECT (canvas), "button_press_event",
                    G_CALLBACK (event_press), NULL);
  g_signal_connect (G_OBJECT (canvas), "button_release_event",
                    G_CALLBACK (event_release), NULL);
  g_signal_connect (G_OBJECT (canvas), "key_press_event",
                    G_CALLBACK (event_keypress), NULL);
  g_object_set (G_OBJECT (canvas), "can_focus", 1, NULL);

  gtk_container_add (GTK_CONTAINER (mainwin), canvas);

  init ();
  gtk_widget_show_all (mainwin);

  g_print ("press X to change between black and white ink\n");
  
  gtk_main ();
  return 0;
}


/* utility function that creates a smooth path from a set
 * of coordinates
 */
static void
points_to_bezier_path (cairo_t *cr,
                       gdouble  coord_x[],
                       gdouble  coord_y[],
                       gint     n_coords)
{
  gint    i;
  gdouble smooth_value;
 
  smooth_value  = 1;

  cairo_new_path (cr);

  if (!n_coords)
    return;

  cairo_move_to (cr, coord_x[0], coord_y[0]);

  for (i=1;i<n_coords;i++)
    {
      gdouble x2 = coord_x[i];
      gdouble y2 = coord_y[i];

      gdouble x0,y0,x1,y1,x3,y3;

      if (i==1)
        {
          x0=coord_x[i-1];
          y0=coord_y[i-1];
          x1 = coord_x[i-1];
          y1 = coord_y[i-1];
        }
      else
        {
          x0=coord_x[i-2];
          y0=coord_y[i-2];
          x1 = coord_x[i-1];
          y1 = coord_y[i-1];
        }

      if (i<n_coords+1)
        {
          x3 = coord_x[i+1];
          y3 = coord_y[i+1];
        }
      else
        {
          x3 = coord_x[i];
          y3 = coord_y[i];
        }

      {
        gdouble xc1 = (x0 + x1) / 2.0;
        gdouble yc1 = (y0 + y1) / 2.0;
        gdouble xc2 = (x1 + x2) / 2.0;
        gdouble yc2 = (y1 + y2) / 2.0;
        gdouble xc3 = (x2 + x3) / 2.0;
        gdouble yc3 = (y2 + y3) / 2.0;

        gdouble len1 = sqrt( (x1-x0) * (x1-x0) + (y1-y0) * (y1-y0) );
        gdouble len2 = sqrt( (x2-x1) * (x2-x1) + (y2-y1) * (y2-y1) );
        gdouble len3 = sqrt( (x3-x2) * (x3-x2) + (y3-y2) * (y3-y2) );

        gdouble k1 = len1 / (len1 + len2);
        gdouble k2 = len2 / (len2 + len3);

        gdouble xm1 = xc1 + (xc2 - xc1) * k1;
        gdouble ym1 = yc1 + (yc2 - yc1) * k1;

        gdouble xm2 = xc2 + (xc3 - xc2) * k2;
        gdouble ym2 = yc2 + (yc3 - yc2) * k2;

        gdouble ctrl1_x = xm1 + (xc2 - xm1) * smooth_value + x1 - xm1;
        gdouble ctrl1_y = ym1 + (yc2 - ym1) * smooth_value + y1 - ym1;

        gdouble ctrl2_x = xm2 + (xc2 - xm2) * smooth_value + x2 - xm2;
        gdouble ctrl2_y = ym2 + (yc2 - ym2) * smooth_value + y2 - ym2;

        cairo_curve_to (cr, ctrl1_x, ctrl1_y, ctrl2_x, ctrl2_y, x2,y2);
      }
   }
}


/* this function is included, just to be
 * able to check the improvement over the easiest method
 */
static void
points_to_linear_path (cairo_t *cr,
                       gdouble  coord_x[],
                       gdouble  coord_y[],
                       gint     n_coords)
{
  gint i;

  if (!n_coords)
    return;
  cairo_move_to (cr, coord_x[0], coord_y[0]);

  for (i = 1; i < n_coords; i++)
    cairo_line_to (cr, coord_x[i], coord_y[i]);
}

