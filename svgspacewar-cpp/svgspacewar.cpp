// SVG Spacewar is copyright 2005 by Nigel Tao: nigel.tao@myrealbox.com
// Licenced under the GNU GPL.
// Developed on cairo version 0.4.0.
//
// 2005-03-31: Version 0.1.

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <sys/timeb.h>

#define WIDTH  800
#define HEIGHT 600

#define TWO_PI (2*M_PI)
#define PI     (M_PI)

// trig computations (and x, y, velocity, etc). are made in fixed point arithmetic
#define FIXED_POINT_SCALE_FACTOR 1024
#define FIXED_POINT_HALF_SCALE_FACTOR 32

// discretization of 360 degrees
#define NUMBER_OF_ROTATION_ANGLES 60
#define RADIANS_PER_ROTATION_ANGLE (TWO_PI / NUMBER_OF_ROTATION_ANGLES)

// equivalent to 25 fps
#define MILLIS_PER_FRAME 40

// a shot every 9/25 seconds = 8 ticks between shots
#define TICKS_BETWEEN_FIRE 8

// fudge this for bigger or smaller ships
#define GLOBAL_SHIP_SCALE_FACTOR 0.8

#define SHIP_ACCELERATION_FACTOR 1
#define SHIP_MAX_VELOCITY (10 * FIXED_POINT_SCALE_FACTOR)
#define SHIP_RADIUS ((int) (38 * FIXED_POINT_SCALE_FACTOR * GLOBAL_SHIP_SCALE_FACTOR))

#define SHIP_MAX_ENERGY 1000
#define DAMAGE_PER_MISSILE 200
#define ENERGY_PER_MISSILE 10

// bounce damage depends on how fast you're going
#define DAMAGE_PER_SHIP_BOUNCE_DIVISOR 3

#define NUMBER_OF_STARS 20

#define MAX_NUMBER_OF_MISSILES 60

#define MISSILE_RADIUS (4 * FIXED_POINT_SCALE_FACTOR)
#define MISSILE_SPEED 8
#define MISSILE_TICKS_TO_LIVE 60
#define MISSILE_EXPLOSION_TICKS_TO_LIVE 6

//------------------------------------------------------------------------------

typedef struct
{
  gdouble r, g, b;
}
RGB_t;

typedef struct
{
  int x, y;
  int vx, vy;

  // 0 is straight up, (NUMBER_OF_ROTATION_ANGLES / 4) is pointing right
  int rotation;

  // used for collision detection - we presume that an object is equivalent
  // to its bounding circle, rather than trying to do something fancy.
  int radius;
}
physics_t;

typedef struct
{
  physics_t p;

  gboolean is_thrusting;
  gboolean is_turning_left;
  gboolean is_turning_right;
  gboolean is_firing;

  RGB_t primary_color;
  RGB_t secondary_color;

  int ticks_until_can_fire;
  int energy;

  gboolean is_hit;
  gboolean is_dead;
}
player_t;

typedef struct
{
  gboolean is_alive;

  physics_t p;

  RGB_t primary_color;
  RGB_t secondary_color;

  int ticks_to_live;
  gboolean has_exploded;
}
missile_t;

typedef struct
{
  int x, y;
  float rotation;
  float scale;
}
star_t;

//------------------------------------------------------------------------------
// Forward definitions of functions

static void apply_physics (physics_t *);
static void apply_physics_to_player (player_t *);
static gboolean check_for_collision (physics_t *, physics_t *);
static void draw_energy_bar (cairo_t *, player_t *);
static void draw_flare (cairo_t *, RGB_t);
static void draw_missile (cairo_t *, missile_t *);
static void draw_exploded_missile (cairo_t *, missile_t *);
static void draw_ship_body (cairo_t *, player_t *);
static void draw_star (cairo_t * cr);
static void draw_turning_flare (cairo_t *, RGB_t, int);
static void enforce_minimum_distance (physics_t *, physics_t *);
static long get_time_millis (void);
static void init_missiles_array (void);
static void init_stars_array (void);
static void init_trigonometric_tables (void);
static void on_collision (player_t *, missile_t *);
static gint on_expose_event (GtkWidget *, GdkEventExpose *);
static gint on_key_event (GtkWidget *, GdkEventKey *, gboolean);
static gint on_key_press (GtkWidget *, GdkEventKey *);
static gint on_key_release (GtkWidget *, GdkEventKey *);
static gint on_timeout (gpointer);
static void reset ();
static void scale_for_aspect_ratio (cairo_t *, int, int);
static void show_text_message (cairo_t *, int, int, const char *);

//------------------------------------------------------------------------------

static player_t player1;
static player_t player2;

//------------------------------------------------------------------------------

static missile_t missiles[MAX_NUMBER_OF_MISSILES];
static int next_missile_index = 0;

static void
init_missiles_array ()
{
  int i;

  for (i = 0; i < MAX_NUMBER_OF_MISSILES; i++)
    {
      missiles[i].p.radius = MISSILE_RADIUS;
      missiles[i].is_alive = FALSE;
    }
}

//------------------------------------------------------------------------------

static star_t stars[NUMBER_OF_STARS];

static void
init_stars_array ()
{
  int i;

  for (i = 0; i < NUMBER_OF_STARS; i++)
    {
      stars[i].x = random () % WIDTH;
      stars[i].y = random () % HEIGHT;
      stars[i].rotation = drand48 () * TWO_PI;
      stars[i].scale = 0.5 + (drand48 ());
    }
}

//------------------------------------------------------------------------------

static gboolean show_fps = TRUE;
static int number_of_frames = 0;
static long millis_taken_for_frames = 0;
static float debug_scale_factor = 1.0f;
static const char *game_over_message = NULL;

//------------------------------------------------------------------------------

static int cos_table[NUMBER_OF_ROTATION_ANGLES];
static int sin_table[NUMBER_OF_ROTATION_ANGLES];

static void
init_trigonometric_tables ()
{
  int i;
  int q = (NUMBER_OF_ROTATION_ANGLES / 4);

  for (i = 0; i < NUMBER_OF_ROTATION_ANGLES; i++)
    {
      // our angle system is "true north" - 0 is straight up, whereas
      // cos & sin take 0 as east (and in radians).
      double angle_in_radians = (q - i) * TWO_PI / NUMBER_OF_ROTATION_ANGLES;
      cos_table[i] =
	+(int) (cos (angle_in_radians) * FIXED_POINT_SCALE_FACTOR);

      // also, our graphics system is "y axis down", although in regular math,
      // the y axis is "up", so we have to multiply sin by -1.
      sin_table[i] =
	-(int) (sin (angle_in_radians) * FIXED_POINT_SCALE_FACTOR);
    }
}

//------------------------------------------------------------------------------

gint
main (gint argc, gchar ** argv)
{
  GtkWidget *window;

  srand ((unsigned int) time (NULL));

  init_trigonometric_tables ();
  reset ();

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_signal_connect (G_OBJECT (window), "delete-event",
		    G_CALLBACK (gtk_main_quit), NULL);

  gtk_window_set_default_size (GTK_WINDOW (window), WIDTH, HEIGHT);

  g_signal_connect (G_OBJECT (window), "expose_event",
		    G_CALLBACK (on_expose_event), NULL);
  g_signal_connect (G_OBJECT (window), "key_press_event",
		    G_CALLBACK (on_key_press), NULL);
  g_signal_connect (G_OBJECT (window), "key_release_event",
		    G_CALLBACK (on_key_release), NULL);
  g_timeout_add (MILLIS_PER_FRAME, (GSourceFunc) on_timeout, window);

  gtk_widget_show_all (window);
  gtk_main ();

  return 0;
}

//------------------------------------------------------------------------------

static long
get_time_millis (void)
{
  struct timeb tp;
  ftime (&tp);
  return (long) ((tp.time * 1000) + tp.millitm);
}

//------------------------------------------------------------------------------

static gint
on_expose_event (GtkWidget * widget, GdkEventExpose * event)
{
  cairo_t *cr = gdk_cairo_create (widget->window);
  int i;
  long start_time = 0;
  if (show_fps)
    {
      start_time = get_time_millis ();
    }

  cairo_save (cr);

  scale_for_aspect_ratio (cr, widget->allocation.width,
			  widget->allocation.height);

  cairo_scale (cr, debug_scale_factor, debug_scale_factor);

  /* draw background space color */
  cairo_set_source_rgb (cr, 0.1, 0.0, 0.1);
  cairo_paint (cr);

  // draw any stars...
  for (i = 0; i < NUMBER_OF_STARS; i++)
    {
      cairo_save (cr);
      cairo_translate (cr, stars[i].x, stars[i].y);
      cairo_rotate (cr, stars[i].rotation);
      cairo_scale (cr, stars[i].scale, stars[i].scale);
      draw_star (cr);
      cairo_restore (cr);
    }

  // ... the energy bars...
  cairo_save (cr);
  cairo_translate (cr, 30, 30);
  cairo_rotate (cr, 0);
  draw_energy_bar (cr, &player1);
  cairo_restore (cr);

  cairo_save (cr);
  cairo_translate (cr, WIDTH - 30, 30);
  cairo_rotate (cr, PI);
  draw_energy_bar (cr, &player2);
  cairo_restore (cr);

  // ... the two ships...
  cairo_save (cr);
  cairo_translate (cr, player1.p.x / FIXED_POINT_SCALE_FACTOR,
		   player1.p.y / FIXED_POINT_SCALE_FACTOR);
  cairo_rotate (cr, player1.p.rotation * RADIANS_PER_ROTATION_ANGLE);
  draw_ship_body (cr, &player1);
  cairo_restore (cr);

  cairo_save (cr);
  cairo_translate (cr, player2.p.x / FIXED_POINT_SCALE_FACTOR,
		   player2.p.y / FIXED_POINT_SCALE_FACTOR);
  cairo_rotate (cr, player2.p.rotation * RADIANS_PER_ROTATION_ANGLE);
  draw_ship_body (cr, &player2);
  cairo_restore (cr);

  // ... and any missiles.
  for (i = 0; i < MAX_NUMBER_OF_MISSILES; i++)
    {
      if (missiles[i].is_alive)
	{
	  cairo_save (cr);
	  cairo_translate (cr, missiles[i].p.x / FIXED_POINT_SCALE_FACTOR,
			   missiles[i].p.y / FIXED_POINT_SCALE_FACTOR);
	  cairo_rotate (cr,
			missiles[i].p.rotation * RADIANS_PER_ROTATION_ANGLE);
	  draw_missile (cr, &(missiles[i]));
	  cairo_restore (cr);
	}
    }

  if (game_over_message == NULL)
    {
      if (player1.is_dead)
	{
	  game_over_message = (player2.is_dead) ? "DRAW" : "RED wins";
	}
      else
	{
	  game_over_message = (player2.is_dead) ? "BLUE wins" : NULL;
	}
    }
  if (game_over_message != NULL)
    {
      show_text_message (cr, 80, -30, game_over_message);
      show_text_message (cr, 30, +40, "Press [SPACE] to restart");
    }

  cairo_restore (cr);

  if (show_fps)
    {
      number_of_frames++;
      millis_taken_for_frames += get_time_millis () - start_time;
      if (number_of_frames >= 100)
	{
	  double fps =
	    1000.0 * ((double) number_of_frames) /
	    ((double) millis_taken_for_frames);
	  printf ("%d frames in %ldms (%.3ffps)\n", number_of_frames,
		  millis_taken_for_frames, fps);
	  number_of_frames = 0;
	  millis_taken_for_frames = 0L;
	}
    }

  cairo_destroy (cr);
  return TRUE;
}

//------------------------------------------------------------------------------

static void
scale_for_aspect_ratio (cairo_t * cr, int widget_width, int widget_height)
{
  double scale;
  int playfield_width, playfield_height;
  int tx, ty;
  gboolean is_widget_wider;

  is_widget_wider = (widget_width * HEIGHT) > (WIDTH * widget_height);

  if (is_widget_wider)
    {
      scale = ((double) widget_height) / HEIGHT;
      playfield_width = (WIDTH * widget_height) / HEIGHT;
      playfield_height = widget_height;
      tx = (widget_width - playfield_width) / 2;
      ty = 0;
    }
  else
    {
      scale = ((double) widget_width) / WIDTH;
      playfield_width = widget_width;
      playfield_height = (HEIGHT * widget_width) / WIDTH;
      tx = 0;
      ty = (widget_height - playfield_height) / 2;
    }

  cairo_translate (cr, tx, ty);
  cairo_rectangle (cr, 0, 0, playfield_width, playfield_height);
  cairo_clip (cr);

  cairo_scale (cr, scale, scale);
}

//------------------------------------------------------------------------------

static void
draw_energy_bar (cairo_t * cr, player_t * p)
{
  cairo_pattern_t *pat;
  double alpha = 0.6;

  cairo_save (cr);

  cairo_rectangle (cr, 0, -5, p->energy / 5, 10);

  pat = cairo_pattern_create_linear (0, 0, SHIP_MAX_ENERGY / 5, 0);
  cairo_pattern_add_color_stop_rgba (pat, 0,
				     p->secondary_color.r,
				     p->secondary_color.g,
				     p->secondary_color.b, alpha);
  cairo_pattern_add_color_stop_rgba (pat, 1, p->primary_color.r,
				     p->primary_color.g, p->primary_color.b,
				     alpha);

  cairo_set_source (cr, pat);
  cairo_fill_preserve (cr);
  cairo_pattern_destroy (pat);

  cairo_set_source_rgb (cr, 0, 0, 0);
  cairo_stroke (cr);
  cairo_restore (cr);
}

//------------------------------------------------------------------------------

static void
draw_ship_body (cairo_t * cr, player_t * p)
{
  cairo_pattern_t *pat;

  if (p->is_hit)
    {
      cairo_set_source_rgba (cr, p->primary_color.r, p->primary_color.g,
			     p->primary_color.b, 0.5);
      cairo_arc (cr, 0, 0, SHIP_RADIUS / FIXED_POINT_SCALE_FACTOR, 0, TWO_PI);
      cairo_stroke (cr);
    }

  cairo_save (cr);
  cairo_scale (cr, GLOBAL_SHIP_SCALE_FACTOR, GLOBAL_SHIP_SCALE_FACTOR);

  if (!p->is_dead)
    {

      if (p->is_thrusting)
	{
	  draw_flare (cr, p->primary_color);
	}

      if (p->is_turning_left && !p->is_turning_right)
	{
	  draw_turning_flare (cr, p->primary_color, -1.0);
	}

      if (!p->is_turning_left && p->is_turning_right)
	{
	  draw_turning_flare (cr, p->primary_color, 1.0);
	}
    }

  cairo_move_to (cr, 0, -33);
  cairo_curve_to (cr, 2, -33, 3, -34, 4, -35);
  cairo_curve_to (cr, 8, -10, 6, 15, 15, 15);
  cairo_line_to (cr, 20, 15);
  cairo_line_to (cr, 20, 7);
  cairo_curve_to (cr, 25, 10, 28, 22, 25, 28);
  cairo_curve_to (cr, 20, 26, 8, 24, 0, 24);
  // half way point
  cairo_curve_to (cr, -8, 24, -20, 26, -25, 28);
  cairo_curve_to (cr, -28, 22, -25, 10, -20, 7);
  cairo_line_to (cr, -20, 15);
  cairo_line_to (cr, -15, 15);
  cairo_curve_to (cr, -6, 15, -8, -10, -4, -35);
  cairo_curve_to (cr, -3, -34, -2, -33, 0, -33);

  pat = cairo_pattern_create_linear (-30.0, -30.0, 30.0, 30.0);
  cairo_pattern_add_color_stop_rgba (pat, 0,
				     p->primary_color.r, p->primary_color.g,
				     p->primary_color.b, 1);
  cairo_pattern_add_color_stop_rgba (pat, 1, p->secondary_color.r,
				     p->secondary_color.g,
				     p->secondary_color.b, 1);

  cairo_set_source (cr, pat);
  cairo_fill_preserve (cr);
  cairo_pattern_destroy (pat);

  cairo_set_source_rgb (cr, 0, 0, 0);
  cairo_stroke (cr);
  cairo_restore (cr);
}

//------------------------------------------------------------------------------

static void
draw_flare (cairo_t * cr, RGB_t color)
{
  cairo_pattern_t *pat;

  cairo_save (cr);
  cairo_translate (cr, 0, 22);
  pat = cairo_pattern_create_radial (0, 0, 2, 0, 5, 12);

  cairo_pattern_add_color_stop_rgba (pat, 0.0, color.r, color.g, color.b, 1);
  cairo_pattern_add_color_stop_rgba (pat, 0.3, 1, 1, 1, 1);
  cairo_pattern_add_color_stop_rgba (pat, 1.0, color.r, color.g, color.b, 0);
  cairo_set_source (cr, pat);
  cairo_arc (cr, 0, 0, 20, 0, TWO_PI);
  cairo_fill (cr);
  cairo_pattern_destroy (pat);
  cairo_restore (cr);
}

//------------------------------------------------------------------------------

static void
draw_turning_flare (cairo_t * cr, RGB_t color, int right_hand_side)
{
  cairo_pattern_t *pat;
  cairo_save (cr);

  cairo_translate (cr, -23 * right_hand_side, 28);
  pat = cairo_pattern_create_radial (0, 0, 1, 0, 0, 7);
  cairo_pattern_add_color_stop_rgba (pat, 0.0, 1, 1, 1, 1);
  cairo_pattern_add_color_stop_rgba (pat, 1.0, color.r, color.g, color.b, 0);
  cairo_set_source (cr, pat);
  cairo_arc (cr, 0, 0, 7, 0, TWO_PI);
  cairo_fill (cr);
  cairo_pattern_destroy (pat);

  cairo_translate (cr, 42 * right_hand_side, -22);
  pat = cairo_pattern_create_radial (0, 0, 1, 0, 0, 7);
  cairo_pattern_add_color_stop_rgba (pat, 0.0, 1, 1, 1, 1);
  cairo_pattern_add_color_stop_rgba (pat, 1.0, color.r, color.g, color.b, 0);
  cairo_set_source (cr, pat);
  cairo_arc (cr, 0, 0, 5, 0, TWO_PI);
  cairo_fill (cr);
  cairo_pattern_destroy (pat);

  cairo_restore (cr);
}

//------------------------------------------------------------------------------

static void
draw_missile (cairo_t * cr, missile_t * m)
{
  cairo_save (cr);
  cairo_scale (cr, GLOBAL_SHIP_SCALE_FACTOR, GLOBAL_SHIP_SCALE_FACTOR);

  if (m->has_exploded)
    {
      draw_exploded_missile (cr, m);
    }
  else
    {
      cairo_pattern_t *pat;

      double alpha = ((double) m->ticks_to_live) / MISSILE_TICKS_TO_LIVE;
      // non-linear scaling so things don't fade out too fast
      alpha = 1.0 - (1.0 - alpha) * (1.0 - alpha);

      cairo_save (cr);
      cairo_move_to (cr, 0, -4);
      cairo_curve_to (cr, 3, -4, 4, -2, 4, 0);
      cairo_curve_to (cr, 4, 4, 2, 10, 0, 18);
      // half way point
      cairo_curve_to (cr, -2, 10, -4, 4, -4, 0);
      cairo_curve_to (cr, -4, -2, -3, -4, 0, -4);

      pat = cairo_pattern_create_linear (0.0, -5.0, 0.0, 5.0);
      cairo_pattern_add_color_stop_rgba (pat, 0,
					 m->primary_color.r,
					 m->primary_color.g,
					 m->primary_color.b, alpha);
      cairo_pattern_add_color_stop_rgba (pat, 1, m->secondary_color.r,
					 m->secondary_color.g,
					 m->secondary_color.b, alpha);

      cairo_set_source (cr, pat);
      cairo_fill (cr);
      cairo_pattern_destroy (pat);
      cairo_restore (cr);

      cairo_save (cr);
      cairo_arc (cr, 0, 0, 3, 0, TWO_PI);

      pat = cairo_pattern_create_linear (0, 3, 0, -3);
      cairo_pattern_add_color_stop_rgba (pat, 0,
					 m->primary_color.r,
					 m->primary_color.g,
					 m->primary_color.b, alpha);
      cairo_pattern_add_color_stop_rgba (pat, 1, m->secondary_color.r,
					 m->secondary_color.g,
					 m->secondary_color.b, alpha);

      cairo_set_source (cr, pat);
      cairo_fill (cr);
      cairo_pattern_destroy (pat);
      cairo_restore (cr);
    }

  cairo_restore (cr);
}

//------------------------------------------------------------------------------

static void
draw_exploded_missile (cairo_t * cr, missile_t * m)
{
  double alpha;
  cairo_pattern_t *pat;

  cairo_save (cr);
  cairo_scale (cr, GLOBAL_SHIP_SCALE_FACTOR, GLOBAL_SHIP_SCALE_FACTOR);

  alpha = ((double) m->ticks_to_live) / MISSILE_EXPLOSION_TICKS_TO_LIVE;
  alpha = 1.0 - (1.0 - alpha) * (1.0 - alpha);

  cairo_arc (cr, 0, 0, 30, 0, TWO_PI);

  pat = cairo_pattern_create_radial (0, 0, 0, 0, 0, 30);
  cairo_pattern_add_color_stop_rgba (pat, 0,
				     m->primary_color.r, m->primary_color.g,
				     m->primary_color.b, alpha);
  cairo_pattern_add_color_stop_rgba (pat, 0.5, m->secondary_color.r,
				     m->secondary_color.g,
				     m->secondary_color.b, alpha * 0.75);
  cairo_pattern_add_color_stop_rgba (pat, 1, 0, 0, 0, 0);

  cairo_set_source (cr, pat);
  cairo_fill (cr);
  cairo_pattern_destroy (pat);
  cairo_restore (cr);
}

//------------------------------------------------------------------------------

static void
draw_star (cairo_t * cr)
{
  int a = NUMBER_OF_ROTATION_ANGLES / 10;
  float r1 = 5.0;
  float r2 = 2.0;
  float c;
  int i;

  cairo_save (cr);
  cairo_move_to (cr, r1 * cos_table[0] / FIXED_POINT_SCALE_FACTOR,
		 r1 * sin_table[0] / FIXED_POINT_SCALE_FACTOR);

  for (i = 0; i < 5; i++) {
    cairo_line_to (cr, r1 * cos_table[0] / FIXED_POINT_SCALE_FACTOR,
		   r1 * sin_table[0] / FIXED_POINT_SCALE_FACTOR);
    cairo_line_to (cr, r2 * cos_table[a] / FIXED_POINT_SCALE_FACTOR,
		   r2 * sin_table[a] / FIXED_POINT_SCALE_FACTOR);
    cairo_rotate (cr, 4*a*PI/NUMBER_OF_ROTATION_ANGLES);
  }

  cairo_close_path (cr);
  cairo_restore (cr);

  c = 0.5;
  cairo_set_source_rgb (cr, c, c, c);
  cairo_fill (cr);
}

//------------------------------------------------------------------------------

static gint
on_timeout (gpointer data)
{
  int i;

  player1.is_hit = FALSE;
  player2.is_hit = FALSE;

  apply_physics_to_player (&player1);
  apply_physics_to_player (&player2);

  if (check_for_collision (&(player1.p), &(player2.p)))
    {
      int p1vx;
      int p1vy;
      int p2vx;
      int p2vy;

      int dvx;
      int dvy;
      int dv2;
      int damage;

      enforce_minimum_distance (&(player1.p), &(player2.p));

      p1vx = player1.p.vx;
      p1vy = player1.p.vy;
      p2vx = player2.p.vx;
      p2vy = player2.p.vy;

      dvx = (p1vx - p2vx) / FIXED_POINT_HALF_SCALE_FACTOR;
      dvy = (p1vy - p2vy) / FIXED_POINT_HALF_SCALE_FACTOR;
      dv2 = (dvx * dvx) + (dvy * dvy);
      damage = ((int)(sqrt (dv2))) / DAMAGE_PER_SHIP_BOUNCE_DIVISOR;

      player1.energy -= damage;
      player2.energy -= damage;
      player1.is_hit = TRUE;
      player2.is_hit = TRUE;

      player1.p.vx = (p1vx * -2 / 8) + (p2vx * +5 / 8);
      player1.p.vy = (p1vy * -2 / 8) + (p2vy * +5 / 8);
      player2.p.vx = (p1vx * +5 / 8) + (p2vx * -2 / 8);
      player2.p.vy = (p1vy * +5 / 8) + (p2vy * -2 / 8);
    }

  for (i = 0; i < MAX_NUMBER_OF_MISSILES; i++)
    {
      if (missiles[i].is_alive)
	{
	  apply_physics (&(missiles[i].p));

	  if (!missiles[i].has_exploded)
	    {
	      if (check_for_collision (&(missiles[i].p), &(player1.p)))
		{
		  on_collision (&player1, &(missiles[i]));
		}

	      if (check_for_collision (&(missiles[i].p), &(player2.p)))
		{
		  on_collision (&player2, &(missiles[i]));
		}
	    }

	  missiles[i].ticks_to_live--;
	  if (missiles[i].ticks_to_live <= 0)
	    {
	      missiles[i].is_alive = FALSE;
	    }
	}
    }

  if (player1.energy <= 0)
    {
      player1.energy = 0;
      player1.is_dead = TRUE;
    }
  else
    {
      player1.energy = MIN (SHIP_MAX_ENERGY, player1.energy + 1);
    }

  if (player2.energy <= 0)
    {
      player2.energy = 0;
      player2.is_dead = TRUE;
    }
  else
    {
      player2.energy = MIN (SHIP_MAX_ENERGY, player2.energy + 1);
    }

  gtk_widget_queue_draw ((GtkWidget *) data);
  return TRUE;
}

//------------------------------------------------------------------------------

static void
apply_physics_to_player (player_t * player)
{
  int v2, m2;
  physics_t *p = &(player->p);

  if (!player->is_dead)
    {
      // check if player is turning left, ...
      if (player->is_turning_left)
	{
	  p->rotation--;
	  while (p->rotation < 0)
	    {
	      p->rotation += NUMBER_OF_ROTATION_ANGLES;
	    }
	}

      // ... or right.
      if (player->is_turning_right)
	{
	  p->rotation++;
	  while (p->rotation >= NUMBER_OF_ROTATION_ANGLES)
	    {
	      p->rotation -= NUMBER_OF_ROTATION_ANGLES;
	    }
	}

      // check if accelerating
      if (player->is_thrusting)
	{
	  p->vx += SHIP_ACCELERATION_FACTOR * cos_table[p->rotation];
	  p->vy += SHIP_ACCELERATION_FACTOR * sin_table[p->rotation];
	}

      // apply velocity upper bound
      v2 = ((p->vx) * (p->vx)) + ((p->vy) * (p->vy));
      m2 = SHIP_MAX_VELOCITY * SHIP_MAX_VELOCITY;
      if (v2 > m2)
	{
	  p->vx = (int) (((double) (p->vx) * m2) / v2);
	  p->vy = (int) (((double) (p->vy) * m2) / v2);
	}

      // check if player is shooting
      if (player->ticks_until_can_fire == 0)
	{
	  if ((player->is_firing) && (player->energy > ENERGY_PER_MISSILE))
	    {
	      int xx = cos_table[p->rotation];
	      int yy = sin_table[p->rotation];

	      missile_t *m = &(missiles[next_missile_index++]);

	      player->energy -= ENERGY_PER_MISSILE;

	      if (next_missile_index == MAX_NUMBER_OF_MISSILES)
		{
		  next_missile_index = 0;
		}

	      m->p.x =
		p->x +
		(((SHIP_RADIUS +
		   MISSILE_RADIUS) / FIXED_POINT_SCALE_FACTOR) * xx);
	      m->p.y =
		p->y +
		(((SHIP_RADIUS +
		   MISSILE_RADIUS) / FIXED_POINT_SCALE_FACTOR) * yy);
	      m->p.vx = p->vx + (MISSILE_SPEED * xx);
	      m->p.vy = p->vy + (MISSILE_SPEED * yy);
	      m->p.rotation = p->rotation;
	      m->ticks_to_live = MISSILE_TICKS_TO_LIVE;
	      m->primary_color = player->primary_color;
	      m->secondary_color = player->secondary_color;
	      m->is_alive = TRUE;
	      m->has_exploded = FALSE;

	      player->ticks_until_can_fire += TICKS_BETWEEN_FIRE;
	    }
	}
      else
	{
	  player->ticks_until_can_fire--;
	}
    }

  // apply velocity deltas to displacement  
  apply_physics (p);
}

//------------------------------------------------------------------------------

static void
apply_physics (physics_t * p)
{
  p->x += p->vx;
  while (p->x > (WIDTH * FIXED_POINT_SCALE_FACTOR))
    {
      p->x -= (WIDTH * FIXED_POINT_SCALE_FACTOR);
    }
  while (p->x < 0)
    {
      p->x += (WIDTH * FIXED_POINT_SCALE_FACTOR);
    }

  p->y += p->vy;
  while (p->y > (HEIGHT * FIXED_POINT_SCALE_FACTOR))
    {
      p->y -= (HEIGHT * FIXED_POINT_SCALE_FACTOR);
    }
  while (p->y < 0)
    {
      p->y += (HEIGHT * FIXED_POINT_SCALE_FACTOR);
    }
}

//------------------------------------------------------------------------------

static gboolean
check_for_collision (physics_t * p1, physics_t * p2)
{
  int dx = (p1->x - p2->x) / FIXED_POINT_HALF_SCALE_FACTOR;
  int dy = (p1->y - p2->y) / FIXED_POINT_HALF_SCALE_FACTOR;
  int r = (p1->radius + p2->radius) / FIXED_POINT_HALF_SCALE_FACTOR;
  int d2 = (dx * dx) + (dy * dy);
  return (d2 < (r * r)) ? TRUE : FALSE;
}

//------------------------------------------------------------------------------

static void
enforce_minimum_distance (physics_t * p1, physics_t * p2)
{
  int dx = p1->x - p2->x;
  int dy = p1->y - p2->y;
  double d2 = (((double) dx) * dx) + (((double) dy) * dy);
  int d = (int) sqrt (d2);

  int r = p1->radius + p2->radius;

  // normalize dx and dy to length = ((r - d) / 2) + fudge_factor
  int desired_vector_length = ((r - d) * 5) / 8;

  dx *= desired_vector_length;
  dy *= desired_vector_length;
  dx /= d;
  dy /= d;

  p1->x += dx;
  p1->y += dy;
  p2->x -= dx;
  p2->y -= dy;
}

//------------------------------------------------------------------------------

static void
on_collision (player_t * p, missile_t * m)
{
  p->energy -= DAMAGE_PER_MISSILE;
  p->is_hit = TRUE;
  m->has_exploded = TRUE;
  m->ticks_to_live = MISSILE_EXPLOSION_TICKS_TO_LIVE;
  m->p.vx = 0;
  m->p.vy = 0;
}

//------------------------------------------------------------------------------

static void
show_text_message (cairo_t * cr, int font_size, int dy, const char *message)
{
  double x, y;
  cairo_text_extents_t extents;

  cairo_save (cr);

  cairo_select_font_face (cr, "Serif",
			  CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);

  cairo_set_font_size (cr, font_size);
  cairo_text_extents (cr, message, &extents);
  x = (WIDTH / 2) - (extents.width / 2 + extents.x_bearing);
  y = (HEIGHT / 2) - (extents.height / 2 + extents.y_bearing);

  cairo_set_source_rgba (cr, 1, 1, 1, 1);
  cairo_move_to (cr, x, y + dy);
  cairo_show_text (cr, message);
  cairo_restore (cr);
}

//------------------------------------------------------------------------------

static void
reset ()
{
  player1.p.x = 200 * FIXED_POINT_SCALE_FACTOR;
  player1.p.y = 200 * FIXED_POINT_SCALE_FACTOR;
  player1.p.vx = 0;
  player1.p.vy = 0;
  player1.p.rotation = random () % NUMBER_OF_ROTATION_ANGLES;
  player1.p.radius = SHIP_RADIUS;
  player1.is_thrusting = FALSE;
  player1.is_turning_left = FALSE;
  player1.is_turning_right = FALSE;
  player1.is_firing = FALSE;
  player1.primary_color.r = 0.3;
  player1.primary_color.g = 0.5;
  player1.primary_color.b = 0.9;
  player1.secondary_color.r = 0.1;
  player1.secondary_color.g = 0.3;
  player1.secondary_color.b = 0.3;
  player1.ticks_until_can_fire = 0;
  player1.energy = SHIP_MAX_ENERGY;
  player1.is_hit = FALSE;
  player1.is_dead = FALSE;

  player2.p.x = 600 * FIXED_POINT_SCALE_FACTOR;
  player2.p.y = 400 * FIXED_POINT_SCALE_FACTOR;
  player2.p.vx = 0;
  player2.p.vy = 0;
  player2.p.rotation = random () % NUMBER_OF_ROTATION_ANGLES;
  player2.p.radius = SHIP_RADIUS;
  player2.is_thrusting = FALSE;
  player2.is_turning_left = FALSE;
  player2.is_turning_right = FALSE;
  player2.is_firing = FALSE;
  player2.primary_color.r = 0.9;
  player2.primary_color.g = 0.2;
  player2.primary_color.b = 0.3;
  player2.secondary_color.r = 0.5;
  player2.secondary_color.g = 0.2;
  player2.secondary_color.b = 0.3;
  player2.ticks_until_can_fire = 0;
  player2.energy = SHIP_MAX_ENERGY;
  player2.is_hit = FALSE;
  player2.is_dead = FALSE;

  init_stars_array ();
  init_missiles_array ();

  game_over_message = NULL;
}

//------------------------------------------------------------------------------

static gint
on_key_press (GtkWidget * widget, GdkEventKey * event)
{
  return on_key_event (widget, event, TRUE);
}

//------------------------------------------------------------------------------

static gint
on_key_release (GtkWidget * widget, GdkEventKey * event)
{
  return on_key_event (widget, event, FALSE);
}

//------------------------------------------------------------------------------

static gint
on_key_event (GtkWidget * widget, GdkEventKey * event, gboolean key_is_on)
{
  switch (event->keyval)
    {
    case GDK_Escape:
      gtk_main_quit ();
      break;

    case GDK_bracketleft:
      if (key_is_on)
	{
	  debug_scale_factor /= 1.25f;
	  printf ("Scale: %f\n", debug_scale_factor);
	}
      break;
    case GDK_bracketright:
      if (key_is_on)
	{
	  debug_scale_factor *= 1.25f;
	  printf ("Scale: %f\n", debug_scale_factor);
	}
      break;

    case GDK_space:
      if (game_over_message != NULL)
	{
	  reset ();
	}
      break;

    case GDK_a:
      player1.is_turning_left = key_is_on;
      break;
    case GDK_d:
      player1.is_turning_right = key_is_on;
      break;
    case GDK_w:
      player1.is_thrusting = key_is_on;
      break;
    case GDK_Control_L:
      player1.is_firing = key_is_on;
      break;

    case GDK_Left:
    case GDK_KP_Left:
      player2.is_turning_left = key_is_on;
      break;
    case GDK_Right:
    case GDK_KP_Right:
      player2.is_turning_right = key_is_on;
      break;
    case GDK_Up:
    case GDK_KP_Up:
      player2.is_thrusting = key_is_on;
      break;
    case GDK_Control_R:
    case GDK_KP_Insert:
      player2.is_firing = key_is_on;
      break;
    }
  return TRUE;
}
