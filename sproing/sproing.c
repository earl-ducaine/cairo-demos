/* -*- mode: c; c-basic-offset: 2 -*- */

#include <gtk/gtk.h>
#include <cairo.h>
#include <cairo-xlib.h>
#include <gdk/gdkx.h>
#include <math.h>

typedef struct _xy_pair Point;
typedef struct _xy_pair Vector;
struct _xy_pair {
  double x, y;
};

typedef struct _Model Model;
typedef struct _Object Object;
typedef struct _Attractor Attractor;
typedef struct _Spring Spring;

#define MODEL_MAX_SPRINGS 50
#define MASS_INFINITE -1.0

#define DEFAULT_SPRING_K 15.0
#define DEFAULT_FRICTION  4.2

#define WALL_X 800
#define WALL_Y 600

struct _Spring {
  Object *a;
  Object *b;
  /* Spring position at rest, from a to b:
	offset = b.position - a.position
  */
  Vector offset;
};

struct _Attractor {
  Object *object;
  Vector offset;
};

struct _Object {
  Vector force;

  Point position;
  Vector velocity;

  double mass;
  double theta;

  int immobile;
};

struct _Model {
  int num_objects;
  Object *objects;

  int num_springs;
  Spring springs[MODEL_MAX_SPRINGS];

  Object *anchor_object;

  double friction;	/* Friction constant */
  double k;		/* Spring constant */
};

static void
object_init (Object *object,
	     double position_x, double position_y,
	     double velocity_x, double velocity_y, double mass)
{
  object->position.x = position_x;
  object->position.y = position_y;

  object->velocity.x = velocity_x;
  object->velocity.y = velocity_y;

  object->mass = mass;

  object->force.x = 0;
  object->force.y = 0;

  object->immobile = 0;
}

static void
spring_init (Spring *spring,
	     Object *object_a, Object *object_b,
	     double offset_x, double offset_y)
{
  spring->a = object_a;
  spring->b = object_b;
  spring->offset.x = offset_x;
  spring->offset.y = offset_y;
}

static void
model_add_spring (Model *model,
		  Object *object_a, Object *object_b,
		  double offset_x, double offset_y)
{
  Spring *spring;

  g_assert (model->num_springs < MODEL_MAX_SPRINGS);

  spring = &model->springs[model->num_springs];
  model->num_springs++;

  spring_init (spring, object_a, object_b, offset_x, offset_y);
}

static void
model_init_grid (Model *model, int width, int height)
{
  int x, y, i;
  const int hpad = 50, vpad = 50;

  model->objects = g_new (Object, width * height);
  model->num_objects = width * height;

  model->num_springs = 0;

  i = 0;
  for (y = 0; y < height; y++)
    for (x = 0; x < width; x++) {
      object_init (&model->objects[i], 200, 150, 0, 0, 20);

#define CX 0
#define CY 0

#if 0
      if (x < CX)
	model_add_spring (model,
			  &model->objects[i],
			  &model->objects[i + 1],
			  -hpad, 0);
      if (x > CX)
	model_add_spring (model,
			  &model->objects[i],
			  &model->objects[i - 1],
			  hpad, 0);

      if (y < CY)
	model_add_spring (model,
			  &model->objects[i],
			  &model->objects[i + width],
			  0, -vpad);
      if (y > CY)
	model_add_spring (model,
			  &model->objects[i],
			  &model->objects[i - width],
			  0, vpad);
#endif

#if 1
      if (x > 0)
	model_add_spring (model,
			  &model->objects[i - 1],
			  &model->objects[i],
			  hpad, 0);

      if (y > 0)
	model_add_spring (model,
			  &model->objects[i - width],
			  &model->objects[i],
			  0, vpad);
#endif
#if 0
      if (x < width - 1)
	model_add_spring (model,
			  &model->objects[i],
			  &model->objects[i + 1],
			  -hpad, 0);

      if (y < height - 1)
	model_add_spring (model,
			  &model->objects[i],
			  &model->objects[i + width],
			  0, -vpad);
#endif

      i++;
    }
}

static void
model_init (Model *model)
{
  model->anchor_object = NULL;

  model->k        = DEFAULT_SPRING_K;
  model->friction = DEFAULT_FRICTION;

  model_init_grid (model, 4, 4);
}

static void
object_apply_force (Object *object, double fx, double fy)
{
  object->force.x += fx;
  object->force.y += fy;
}

/* The model here can be understood as a rigid body of the spring's
 * rest shape, centered on the vector between the two object
 * positions. This rigid body is then connected by linear-force
 * springs to each object. This model does degnerate into a simple
 * spring for linear displacements, and does something reasonable for
 * rotation.
 *
 * There are other possibilities for handling the rotation of the
 * spring, and it might be interesting to explore something which has
 * better length-preserving properties. For example, with the current
 * model, an initial 180 degree rotation of the spring results in the
 * spring collapsing down to 0 size before expanding back to it's
 * natural size again.
 */
static void
spring_exert_forces (Spring *spring, double k)
{
  Vector da, db;
  Vector a, b;

  a = spring->a->position;
  b = spring->b->position;

  /* A nice vector diagram would likely help here, but my ASCII-art
   * skills aren't up to the task. Here's how to make your own
   * diagram:
   *
   * Draw a and b, and the vector AB from a to b
   * Find the center of AB
   * Draw spring->offset so that its center point is on the center of AB
   * Draw da from a to the initial point of spring->offset
   * Draw db from b to the final point of spring->offset
   *
   * The math below should be easy to verify from the diagram.
   */

  da.x = 0.5 * (b.x - a.x - spring->offset.x);
  da.y = 0.5 * (b.y - a.y - spring->offset.y);

  db.x = 0.5 * (a.x - b.x + spring->offset.x);
  db.y = 0.5 * (a.y - b.y + spring->offset.y);

  object_apply_force (spring->a, k *da.x, k * da.y);

  object_apply_force (spring->b, k * db.x, k * db.y);
}

static void
model_step_object (Model *model, Object *object)
{
  Vector acceleration;

  object->theta += 0.05;

  /* Slow down due to friction. */
  object->force.x -= model->friction * object->velocity.x;
  object->force.y -= model->friction * object->velocity.y;

  acceleration.x = object->force.x / object->mass;
  acceleration.y = object->force.y / object->mass;

  if (object->immobile) {
    object->velocity.x = 0;
    object->velocity.y = 0;
  } else {
    object->velocity.x += acceleration.x;
    object->velocity.y += acceleration.y;

    object->position.x += object->velocity.x;
    object->position.y += object->velocity.y;

    if (object->position.x > WALL_X) {
      object->position.x = WALL_X - (object->position.x - WALL_X) * 0.7;
      object->velocity.x = -object->velocity.x * 0.3;
    }

    if (object->position.y > WALL_Y) {
      object->position.y = WALL_Y - (object->position.y - WALL_Y) * 0.7;
      object->velocity.y = -object->velocity.y * 0.3;
    }
  }

  object->force.x = 0.0;
  object->force.y = 0.0;
}

static void
model_step (Model *model)
{
  int i;

  for (i = 0; i < model->num_springs; i++)
    spring_exert_forces (&model->springs[i], model->k);

  for (i = 0; i < model->num_objects; i++)
    model_step_object (model, &model->objects[i]);
}

static cairo_t *
begin_paint (GdkDrawable *window)
{
    Display *dpy;
    Drawable xid;
    Visual *visual;
    GdkDrawable *drawable;
    gint x_offset, y_offset;
    gint width, height;
    cairo_surface_t *surface;
    cairo_t *cr;

    if (GDK_IS_WINDOW (window))
	gdk_window_get_internal_paint_info (window, &drawable,
					    &x_offset, &y_offset);
    else
	drawable = window;

    dpy = gdk_x11_drawable_get_xdisplay (drawable);
    xid = gdk_x11_drawable_get_xid (drawable);
    gdk_drawable_get_size (drawable, &width, &height);
    visual = GDK_VISUAL_XVISUAL(gdk_drawable_get_visual (drawable));
    surface = cairo_xlib_surface_create (dpy, xid, visual,
					 width, height);
    cr = cairo_create (surface);
    cairo_surface_destroy (surface);

    if (GDK_IS_WINDOW (window))
	cairo_translate (cr, -x_offset, -y_offset);

    return cr;
}

static void
end_paint (cairo_t *cr)
{
    cairo_destroy (cr);
}

typedef struct _Color Color;
struct _Color {
  double red, green, blue;
};

static Color red = { 1, 0, 0 };
static Color blue = { 0, 0, 1 };

static void
draw_ball (GtkWidget   *widget,
	   gdouble      x,
	   gdouble      y,
	   Color        *color)
{
  cairo_t *cr;

  cr = begin_paint (widget->window);

  cairo_set_source_rgba (cr, color->red, color->green, color->blue, 0.5);
  cairo_new_path (cr);
  cairo_arc (cr, x, y, 3, 0, 2 * G_PI);
  cairo_fill (cr);

  cairo_new_path (cr);
  cairo_arc (cr, x, y, 10, 0, 2 * G_PI);
  cairo_stroke (cr);

  end_paint (cr);
}

static void
draw_star (GtkWidget	*widget,
	   gdouble	cx,
	   gdouble	cy,
	   double	theta,
	   Color	*color)
{
  const int spike_count = 5;
  const int inner_radius = 2;
  const int outer_radius = 4;
  cairo_t *cr;
  double x, y;
  int i;

  cr = begin_paint (widget->window);

  cairo_set_source_rgba (cr, color->red, color->green, color->blue, 0.5);
  cairo_new_path (cr);
  for (i = 0; i < spike_count; i++) {
    x = cx + cos ((i * 2) * M_PI / spike_count + theta) * inner_radius;
    y = cy + sin ((i * 2) * M_PI / spike_count + theta) * inner_radius;

    if (i == 0)
      cairo_move_to (cr, x, y);
    else
      cairo_line_to (cr, x, y);

    x = cx + cos ((i * 2 + 1) * M_PI / spike_count + theta) * outer_radius;
    y = cy + sin ((i * 2 + 1) * M_PI / spike_count + theta) * outer_radius;

    cairo_line_to (cr, x, y);
  }
  cairo_fill (cr);

  end_paint (cr);
}

static void
evaluate_bezier_point (Object *objects,
		       double u, double v,
		       double *patch_x, double *patch_y)
{
  double coeffs_u[4], coeffs_v[4];
  double x, y;
  int i, j;

  coeffs_u[0] = (1 - u) * (1 - u) * (1 - u);
  coeffs_u[1] = 3 * u * (1 - u) * (1 - u);
  coeffs_u[2] = 3 * u * u * (1 - u);
  coeffs_u[3] = u * u * u;

  coeffs_v[0] = (1 - v) * (1 - v) * (1 - v);
  coeffs_v[1] = 3 * v * (1 - v) * (1 - v);
  coeffs_v[2] = 3 * v * v * (1 - v);
  coeffs_v[3] = v * v * v;

  x = 0;
  y = 0;
  for (i = 0; i < 4; i++)
    for (j = 0; j < 4; j++)
      {
	x += coeffs_u[i] * coeffs_v[j] * objects[j * 4 + i].position.x;
	y += coeffs_u[i] * coeffs_v[j] * objects[j * 4 + i].position.y;
      }

  *patch_x = x;
  *patch_y = y;
}

static void
draw_spline_grid (GtkWidget	*widget,
		  Model      *model)
{
  cairo_t *cr;
  double u, v;
  double x, y;

  cr = begin_paint (widget->window);

  cairo_set_source_rgb (cr, 0, 0, 0);
  cairo_set_line_width (cr, 1.0);

  cairo_new_path (cr);
  for (u = 0; u <= 1.01; u += 0.2)
    {
      for (v = 0; v <= 1.01; v += 0.05)
	{
	  evaluate_bezier_point (model->objects, u, v, &x, &y);
	  if (v == 0)
	    cairo_move_to (cr, x, y);
	  else
	    cairo_line_to (cr, x, y);
	}
    }

  for (v = 0; v <= 1.01; v += 0.2)
    {
      for (u = 0; u <= 1.01; u += 0.05)
	{
	  evaluate_bezier_point (model->objects, u, v, &x, &y);
	  if (u == 0)
	    cairo_move_to (cr, x, y);
	  else
	    cairo_line_to (cr, x, y);
	}
    }

  cairo_stroke (cr);

  end_paint (cr);
}

#if 0
static void
draw_spline_spiral (GtkWidget	*widget,
		    Model      *model)
{
  cairo_t *cr;
  double u, v, r, phi, offset;
  double x, y;

  cr = begin_paint (widget->window);

  cairo_set_source_rgb (cr, 0, 0, 0);
  cairo_set_line_width (cr, 0.8);

  cairo_new_path (cr);

  offset = 0.01;
  for (phi = 0, r = 0; r < 0.5; phi += 0.1, r += 0.002)
    {
      u = cos (phi) * (r + offset) + 0.5;
      v = sin (phi) * (r + offset)+ 0.5;
      offset = -offset;
      evaluate_bezier_point (model->objects, u, v, &x, &y);
      if (phi < 0.05)
	cairo_move_to (cr, x, y);
      else
	cairo_line_to (cr, x, y);
    }

  cairo_stroke (cr);

  end_paint (cr);
}
#endif

static void
draw_wall (GtkWidget *widget)
{
  cairo_t *cr;

  cr = begin_paint (widget->window);

  cairo_save (cr);
  {
    cairo_set_source_rgb (cr, 0.8, 0.7, 0.7);
    cairo_paint (cr);
  }
  cairo_restore (cr);

  cairo_move_to (cr, 0, WALL_Y);
  cairo_line_to (cr, WALL_X, WALL_Y);
  cairo_line_to (cr, WALL_X, 0);
  cairo_stroke (cr);

  end_paint (cr);
}

static gboolean
sproing_expose_event (GtkWidget      *widget,
		      GdkEventExpose *event,
		      gpointer	      data)
{
  Model *model = data;
  int i;

  draw_wall (widget);

#if 0
  draw_spline_spiral (widget, model);
#else
  draw_spline_grid (widget, model);
#endif

  if (model->anchor_object)
    draw_ball (widget,
	       model->anchor_object->position.x,
	       model->anchor_object->position.y, &red);

#if 1
  for (i = 0; i < model->num_objects; i++) {
    draw_star (widget, model->objects[i].position.x,
	       model->objects[i].position.y, model->objects[i].theta, &blue);
  }
#endif

  return TRUE;
}

static double
object_distance (Object *object, double x, double y)
{
  double dx, dy;

  dx = object->position.x - x;
  dy = object->position.y - y;

  return sqrt (dx*dx + dy*dy);
}

static Object *
model_find_nearest (Model *model, double x, double y)
{
  Object *object;
  double distance, min_distance;
  int i;

  for (i = 0; i < model->num_objects; i++) {
    distance = object_distance (&model->objects[i], x, y);
    if (i == 0 || distance < min_distance) {
      min_distance = distance;
      object = &model->objects[i];
    }
  }

  return object;
}

static gboolean
sproing_button_release_event (GtkWidget	     *widget,
			      GdkEventButton *event,
			      gpointer	      data)
{
  Model *model = data;

  if (event->button != 1)
    return TRUE;

  if (model->anchor_object)
    model->anchor_object->immobile = 0;

  return TRUE;
}

static gboolean
sproing_button_press_event (GtkWidget	   *widget,
			    GdkEventButton *event,
			    gpointer	    data)
{
  Model *model = data;
  double x, y;

  if (event->button != 1)
    return TRUE;

  /* Shift by one-half pixel for better stroke alignment */
  x = event->x + 0.5;
  y = event->y + 0.5;

  if (model->anchor_object)
    model->anchor_object->immobile = 0;

  model->anchor_object = model_find_nearest (model, x, y);

  model->anchor_object->immobile = 1;

  model->anchor_object->position.x = x;
  model->anchor_object->position.y = y;

  return TRUE;
}

static gboolean
sproing_motion_notify_event (GtkWidget	    *widget,
			     GdkEventMotion *event,
			     gpointer	     data)
{
  Model *model = data;
  int x, y;
  GdkModifierType state;

  gdk_window_get_pointer (event->window, &x, &y, &state);

  if (state & GDK_BUTTON1_MASK) {
    model->anchor_object->position.x = x + 0.5;
    model->anchor_object->position.y = y + 0.5;
  }

  return TRUE;
}

static void
spring_constant_changed (GtkSpinButton *spinbutton, gpointer user_data)
{
  Model *model = user_data;

  model->k = gtk_spin_button_get_value (spinbutton);
}

static void
friction_changed (GtkSpinButton *spinbutton, gpointer user_data)
{
  Model *model = user_data;

  model->friction = gtk_spin_button_get_value (spinbutton);
}

static GtkWidget *
create_spinners (Model *model)
{
  GtkWidget *hbox;
  GtkWidget *spinner, *label;

  hbox = gtk_hbox_new (FALSE, 8);

  label = gtk_label_new_with_mnemonic ("_Spring constant:");
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  spinner = gtk_spin_button_new_with_range  (0.05, 30.00, 0.05);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), spinner);
  gtk_box_pack_start (GTK_BOX (hbox), spinner, FALSE, FALSE, 0);
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (spinner), model->k);
  g_signal_connect (spinner, "value-changed",
		    G_CALLBACK (spring_constant_changed), model);

  label = gtk_label_new_with_mnemonic ("_Friction:");
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  spinner = gtk_spin_button_new_with_range  (0.05, 15.00, 0.05);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), spinner);
  gtk_box_pack_start (GTK_BOX (hbox), spinner, FALSE, FALSE, 0);
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (spinner), model->friction);
  g_signal_connect (spinner, "value-changed",
		    G_CALLBACK (friction_changed), model);

  return hbox;
}

static GtkWidget *
create_window (Model *model)
{
  GtkWidget *window;
  GtkWidget *frame;
  GtkWidget *vbox;
  GtkWidget *da;
  GtkWidget *spinners;

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Drawing Area");

  g_signal_connect (window, "destroy",
		    G_CALLBACK (gtk_main_quit), &window);

  gtk_container_set_border_width (GTK_CONTAINER (window), 8);

  vbox = gtk_vbox_new (FALSE, 8);
  gtk_container_set_border_width (GTK_CONTAINER (vbox), 8);
  gtk_container_add (GTK_CONTAINER (window), vbox);

  /*
   * Create the drawing area
   */

  frame = gtk_frame_new (NULL);
  gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);
  gtk_box_pack_start (GTK_BOX (vbox), frame, TRUE, TRUE, 0);

  da = gtk_drawing_area_new ();
  /* set a minimum size */
  gtk_widget_set_size_request (da, 400, 300);

  gtk_container_add (GTK_CONTAINER (frame), da);

  /* Signals used to handle backing pixmap */

  g_signal_connect (da, "expose_event",
		    G_CALLBACK (sproing_expose_event), model);

  /* Event signals */

  g_signal_connect (da, "motion_notify_event",
		    G_CALLBACK (sproing_motion_notify_event), model);
  g_signal_connect (da, "button_press_event",
		    G_CALLBACK (sproing_button_press_event), model);
  g_signal_connect (da, "button_release_event",
		    G_CALLBACK (sproing_button_release_event), model);

  /* Ask to receive events the drawing area doesn't normally
   * subscribe to
   */
  gtk_widget_set_events (da, gtk_widget_get_events (da)
			 | GDK_LEAVE_NOTIFY_MASK
			 | GDK_BUTTON_PRESS_MASK
			 | GDK_BUTTON_RELEASE_MASK
			 | GDK_POINTER_MOTION_MASK
			 | GDK_POINTER_MOTION_HINT_MASK);

  spinners = create_spinners (model);
  gtk_box_pack_start (GTK_BOX (vbox), spinners, FALSE, FALSE, 0);

  return da;
}

typedef struct _Closure Closure;
struct _Closure {
  GtkWidget *drawing_area;
  Model *model;
  int i;
};

static gint
timeout_callback (gpointer data)
{
  Closure *closure = data;

  model_step (closure->model);
  closure->i++;
  if (closure->i == 1) {
    gtk_widget_queue_draw (closure->drawing_area);
    closure->i = 0;
  }

  return TRUE;
}

int
main (int argc, char *argv[])
{
  Closure closure;
  Model model;

  gtk_init (&argc, &argv);
  model_init (&model);
  closure.drawing_area = create_window (&model);
  closure.i = 0;
  gtk_widget_show_all (gtk_widget_get_toplevel (closure.drawing_area));
  closure.model = &model;
  g_timeout_add (100, timeout_callback, &closure);
  gtk_main ();

  return 0;
}
