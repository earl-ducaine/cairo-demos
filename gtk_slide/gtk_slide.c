/* Gtk Slide, a sample game built around the puzzle widget */

/* since the puzzle widget leaves the drawing behavior in a sane state, we can hook up the menu / score
 * display on top of it, this might be considered an evil hack, but might make some programming easier
 */

#include <gtk/gtk.h>
#include "puzzle.h"

#define INITIAL_WIDTH  200
#define INITIAL_HEIGHT 210

GtkWidget *puzzle_window;
GtkWidget *puzzle;

GList  *messages = NULL;
double  y_pos = 0.0;
double  y_inc = 0.1;

double fade      = 0.0;

int    rows      = 3;
int    cols      = 3;
int    shuffles = 15;

gboolean update_messages (gpointer data)
{
  if (messages)
    {
      y_pos += y_inc;
      if (y_pos > 1.0)
      {
        char *msg = messages->data;
        messages = g_list_remove (messages, msg);
        g_free (msg);
        y_pos = 0.0;
      }
      gtk_widget_queue_draw (puzzle);
    }
  return TRUE;
}

void add_message (const char *message)
{
  messages = g_list_append (messages, g_strdup(message));
}

static GtkWidget *
puzzle_window_new (void);

gint
main (gint argc,
      gchar *argv[])
{

  gtk_init (&argc, &argv);

  puzzle_window = puzzle_window_new ();
  gtk_widget_show (puzzle_window);
  add_message ("");
  add_message ("Gtk Slide");
  add_message ("");
  add_message ("order the boxes");
  g_timeout_add (100, update_messages ,puzzle);

  gtk_main ();

  return 0;
}

static void paint_status  (GtkWidget      *wdiget,
                           GdkEventExpose *eev,
                           gpointer        user_data);

static void
puzzle_solved (GtkWidget *widget,
               gpointer   data);

static GtkWidget *
puzzle_window_new (void) {
  GtkWidget *self;
  GtkWidget *vbox;

  self = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (self), "Sliding Gtk Puzzle");
  g_signal_connect (G_OBJECT (self), "delete-event",
                    G_CALLBACK (gtk_main_quit), NULL);

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (vbox), 0);

  puzzle = puzzle_new ();

  g_signal_connect (G_OBJECT (puzzle), "expose-event",
    G_CALLBACK (paint_status), NULL);

  g_object_set (G_OBJECT (puzzle), "rows", rows, NULL);
  g_object_set (G_OBJECT (puzzle), "cols", cols, NULL);
  g_object_set (G_OBJECT (puzzle), "shuffles", shuffles, NULL);

  gtk_widget_set_usize (GTK_WIDGET (puzzle), INITIAL_WIDTH, INITIAL_HEIGHT);

  g_signal_connect (G_OBJECT (puzzle), "puzzle_solved",
                    G_CALLBACK (puzzle_solved), puzzle);

  gtk_container_add (GTK_CONTAINER (vbox), puzzle);

  gtk_container_add (GTK_CONTAINER (self), vbox);
  gtk_widget_show_all (vbox);

  return self;
}


static void paint_status  (GtkWidget      *widget,
                           GdkEventExpose *eev,
                           gpointer        user_data)
{
  cairo_t *cr;
  double w = widget->allocation.width;
  double h = widget->allocation.height;

  if (!messages)  /* bail out early, if nothing to paint */
    return;
  
  cr = gdk_cairo_create (widget->window);
    {
    if (fade>0.01)
      {
        cairo_set_source_rgba (cr, 0,0,0, fade);
        cairo_rectangle (cr, 0, 0, w*1.0, h*1.0);
        cairo_fill (cr);
      }

#define FONT_HEIGHT 0.1
#define LINE_HEIGHT 1.3

    cairo_set_font_size (cr, h*FONT_HEIGHT);
    cairo_select_font_face (cr, "Sans", 0, 0);

      {
        GList *item = messages;
        double x,y;

        /*
        char utf8[42];
        snprintf (utf8, 42, "%i×%i @%i", cols, rows, shuffles);
        */

        y = FONT_HEIGHT * LINE_HEIGHT - y_pos * FONT_HEIGHT * LINE_HEIGHT;
        while (item)
          {
            cairo_text_extents_t extents;
            const char *utf8;

            utf8 = item->data;
            cairo_text_extents (cr, utf8, &extents);
            x = 0.5 - (extents.width / 2 + extents.x_bearing) / w;
            cairo_move_to (cr, w*x, h*y);
            cairo_text_path (cr, utf8);

            cairo_save (cr);
              {
              cairo_set_source_rgba (cr, 0,0,0,0.2);
              cairo_set_line_width (cr, h*0.02);
              cairo_stroke (cr);
              }
            cairo_restore (cr);
            cairo_set_source_rgba (cr, 1,1,1,1.0);
            cairo_fill (cr);

            y+= FONT_HEIGHT * LINE_HEIGHT;
            item = g_list_next (item);
          }

        
      }
    }
  cairo_destroy (cr);
  
}

static void
puzzle_solved (GtkWidget *widget,
               gpointer   data)
{
  GtkWidget *dialog;
  GtkWidget *label;
  GtkWidget *parent;

  parent = gtk_widget_get_toplevel (widget);

  dialog = gtk_dialog_new_with_buttons ("GtkSlide - solved",
                                        GTK_WINDOW (parent),
                                        GTK_DIALOG_MODAL,
                                        GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
                                        NULL);

  label = gtk_label_new ("You solved the puzzle!");
  gtk_misc_set_padding (GTK_MISC (label), 20, 20);
  gtk_widget_show (label);

  gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), label);
  gtk_widget_show (dialog);

  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_main_quit ();
}
