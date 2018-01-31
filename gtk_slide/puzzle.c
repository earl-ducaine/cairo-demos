#include <stdio.h>
#include <stdlib.h>

#include <gtk/gtk.h>
#include "cairo_custom.h"

#include "puzzle.h"
#include "math.h"    /* floor() fabs() */

/** game specific functions, defined at end of this file */

static void
board_init (GtkWidget *widget);

static int
query_pos (Puzzle *puzzle,
           gint    x, 
           gint    y);

static void
push_block (Puzzle *puzzle, 
            gint    block_no, 
            gdouble xdelta,
            gdouble ydelta);

static void
draw_board (Puzzle *puzzle, 
            cairo_t *cr);

static gint
board_solved (Puzzle *puzzle);

/** drawing_area derived widget **/

static GObjectClass *parent_class = NULL;

typedef struct
{
  double x;
  double y;
  char  label[16];
} PuzzleItem;

struct _Puzzle {
  GtkDrawingArea drawing_area;

  double ratio_x;
  double ratio_y;

  double cursorx;
  double cursory;

  PuzzleItem *item;

  gint grabbed;

  gint rows;
  gint cols;
  gint shuffles;
};

struct _PuzzleClass {
  GtkDrawingAreaClass parent_class;
};


enum {
  PUZZLE_SOLVED_SIGNAL,
  LAST_SIGNAL
};

enum {
  PUZZLE_ROWS=1,
  PUZZLE_COLS,
  PUZZLE_SHUFFLES
};

static void
set_property (GObject      *object,
              guint         property_id,
              const GValue *value,
              GParamSpec   *psec)
{
  Puzzle *puzzle = PUZZLE (object);

  switch (property_id)
    {

    case PUZZLE_COLS:
      puzzle->cols = g_value_get_int (value);
      board_init (GTK_WIDGET (puzzle));
      break;

    case PUZZLE_ROWS:
      puzzle->rows = g_value_get_int (value);
      board_init (GTK_WIDGET (puzzle));
      break;

    case PUZZLE_SHUFFLES:
      puzzle->shuffles = g_value_get_int (value);
      board_init (GTK_WIDGET (puzzle));
      break;

    default:
      g_assert (FALSE);
      break;
  }
}

static void
get_property (GObject *object,
                guint  property_id,
               GValue *value,
           GParamSpec *pspec)
{
  Puzzle *puzzle = PUZZLE (object);

  switch (property_id)
    {
    case PUZZLE_ROWS:
      g_value_set_int (value, puzzle->rows);
      break;

    case PUZZLE_COLS:
      g_value_set_int (value, puzzle->rows);
      break;

    case PUZZLE_SHUFFLES:
      g_value_set_int (value, puzzle->shuffles);
      break;

    default:
      g_assert (FALSE);
      break;
  }
}

static void
finalize (GObject *object)
  {

    Puzzle *puzzle = PUZZLE (object);

    if (puzzle->item)
      {
        free (puzzle->item);
      }

    parent_class->finalize (object);
}

static gint puzzle_signals[LAST_SIGNAL] = { 0 };

static void
puzzle_class_init (PuzzleClass * class)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (class);
  parent_class = g_type_class_peek (GTK_TYPE_DRAWING_AREA);

  gobject_class->finalize =    finalize;
  gobject_class->get_property = get_property;
  gobject_class->set_property = set_property;

  g_object_class_install_property (gobject_class,
                                   PUZZLE_ROWS,
                                   g_param_spec_int ("rows",
                                                     "Number of rows",
                                                     "",
                                                     0,
                                                     20,
                                                     0,
                                                     G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class,
                                   PUZZLE_COLS,
                                   g_param_spec_int ("cols",
                                                     "Number of cols",
                                                     "",
                                                     0,
                                                     20,
                                                     0,
                                                     G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class,
                                   PUZZLE_SHUFFLES,
                                   g_param_spec_int ("shuffles",
                                                     "Number of moves shuffling board, (approximate)",
                                                     "",
                                                     0,
                                                     1000000,
                                                     0,
                                                     G_PARAM_READWRITE));

  puzzle_signals[PUZZLE_SOLVED_SIGNAL] =
      g_signal_new ("puzzle_solved",
                    G_TYPE_FROM_CLASS (gobject_class),
                    G_SIGNAL_RUN_FIRST, 0, NULL, NULL,
                    g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0, NULL);
}

static void
puzzle_init (Puzzle * puzzle)
{
  puzzle->cursorx = 0;
  puzzle->cursory = 0;
  puzzle->rows = 4;
  puzzle->cols = 4;
  puzzle->shuffles = 200;
  puzzle->grabbed = -1;
  puzzle->ratio_x = 1;
  puzzle->ratio_y = 1;
  board_init (GTK_WIDGET (puzzle));
  gtk_widget_show_all (GTK_WIDGET (puzzle));
}

GType
puzzle_get_type (void)
{
  static GType ge_type = 0;

  if (!ge_type)
    {
      static const GTypeInfo ge_info =
        {
             sizeof (PuzzleClass),
             NULL,
             NULL,
             (GClassInitFunc) puzzle_class_init,
             NULL,
             NULL,
             sizeof (Puzzle),
             0,
             (GInstanceInitFunc) puzzle_init,
        };

      ge_type = g_type_register_static (GTK_TYPE_DRAWING_AREA,
                                        "Puzzle",
                                        &ge_info,
                                        0);
    }
  return ge_type;
}

/* prototypes for event functions registered with the object */

static gboolean event_press (GtkWidget      *widget,
                             GdkEventButton *bev,
                             gpointer        user_data);

static gboolean event_release (GtkWidget      *widget,
                               GdkEventButton *bev,
                               gpointer        user_data);

static gboolean event_motion (GtkWidget      *widget,
                              GdkEventMotion *mev,
                              gpointer        user_data);

static void puzzle_class_init (PuzzleClass *class);

/* prototpe for the redraw callback we register with drawing_area */

static void paint  (GtkDrawingArea *drawing_area,
		    GdkEventExpose *eev,
                    gpointer  user_data);

GtkWidget *
puzzle_new (void)
{
  GtkWidget *widget = GTK_WIDGET (g_object_new (puzzle_get_type (), NULL));
  Puzzle *puzzle = PUZZLE (widget);

  gtk_widget_set_events (widget,
                         GDK_EXPOSURE_MASK        |
                         GDK_POINTER_MOTION_HINT_MASK |  /* since we do dragging, let the ui update request redraw events */
                         GDK_BUTTON1_MOTION_MASK  |
                         GDK_BUTTON2_MOTION_MASK  |
                         GDK_BUTTON3_MOTION_MASK  |
                         GDK_BUTTON_PRESS_MASK    |
                         GDK_BUTTON_RELEASE_MASK);

  gtk_widget_set_size_request (widget, 256, 256);

  g_signal_connect (G_OBJECT (widget), "expose-event",
                    G_CALLBACK (paint), puzzle);

  g_signal_connect (G_OBJECT (widget), "motion_notify_event",
                    G_CALLBACK (event_motion), puzzle);

  g_signal_connect (G_OBJECT (widget), "button_press_event",
                    G_CALLBACK (event_press), puzzle);

  g_signal_connect (G_OBJECT (widget), "button_release_event",
                    G_CALLBACK (event_release), puzzle);

  return widget;
}

/* the actually user interface event functions */

static gboolean
event_press (GtkWidget      *widget,
             GdkEventButton *bev,
             gpointer        user_data)
{
  Puzzle *puzzle = PUZZLE (user_data);

  puzzle->cursorx = bev->x;
  puzzle->cursory = bev->y;

  switch (bev->button)
    {
      case 1:
        puzzle->grabbed = query_pos (puzzle, bev->x, bev->y);
        /* request a redraw, since we've changed the state of our widget */
        gtk_widget_queue_draw (GTK_WIDGET (puzzle));
        break;
    }
  return FALSE;
}

static gboolean
event_release (GtkWidget * widget,
               GdkEventButton * bev, gpointer user_data) {
    Puzzle *puzzle = PUZZLE (user_data);

    switch (bev->button)
      {
        case 1:
          puzzle->grabbed = -1;

          /* request a redraw, since we've changed the state of our widget */
          gtk_widget_queue_draw (GTK_WIDGET (puzzle));
          break;
      }
    return FALSE;
}

static gboolean
event_motion (GtkWidget      *widget,
              GdkEventMotion *mev,
              gpointer        user_data)
{
    Puzzle *puzzle = PUZZLE (user_data);

    if (puzzle->grabbed>=0)
      {
        double xdelta = (mev->x-puzzle->cursorx) / puzzle->ratio_x;
        double ydelta = (mev->y-puzzle->cursory) / puzzle->ratio_y;

        push_block (puzzle, puzzle->grabbed, xdelta, ydelta);

        if (board_solved (puzzle))
          {
            g_signal_emit (G_OBJECT (puzzle),
                           puzzle_signals
                           [PUZZLE_SOLVED_SIGNAL], 0);
          }


        /* request a redraw, since we've changed the state of our widget */
        gtk_widget_queue_draw (GTK_WIDGET (puzzle));

        puzzle->cursorx = mev->x;
        puzzle->cursory = mev->y;
      }
    return FALSE;
}

/* actual paint function */

static void
paint (GtkDrawingArea *drawing_area,
       GdkEventExpose *eev,
       gpointer  user_data)
{
  GtkWidget *widget = GTK_WIDGET (drawing_area);
  Puzzle *puzzle = PUZZLE (drawing_area);
  cairo_t *cr = gdk_cairo_create (widget->window);

  puzzle->ratio_x = (double) widget->allocation.width/puzzle->cols;
  puzzle->ratio_y = (double) widget->allocation.height/puzzle->rows;

  draw_board (puzzle, cr);

  if (cairo_status (cr))
    {
      fprintf (stderr, "Cairo is unhappy: %s\n",
               cairo_status_to_string (cairo_status (cr)));
    }

  cairo_destroy (cr);

  /* since we are requesting motion hints,. make sure another motion
     event is delivered after redrawing the ui */
  gdk_window_get_pointer (widget->window, NULL, NULL, NULL);
}


/*************************************************************************/
/***         end of GtkDrawingArea example, actual game logic follows        ***/
/*************************************************************************/


static void
get_empty (Puzzle *puzzle,
           gint   *col,
           gint   *row);

static int
get_at (Puzzle *puzzle,
        gint    col,
        gint    row);

/* shuffle an initialized board (important, do not
   shuffle a board where the coordinates have changed
   since initialization, since the shuffeling presumes that
   the order haven't changed.
*/

#include <assert.h>

static
void puzzle_shuffle (Puzzle *puzzle,
                     gint    shuffles)
{
  while (shuffles--)
    {
      gint empty_x;
      gint empty_y;
      gint block_no;
      gint direction = g_random_int_range (0, 5);

      get_empty (puzzle, &empty_x, &empty_y);

      switch (direction)
        {
        case 0:
          if ((block_no=get_at (puzzle, empty_x-1, empty_y))>=0)
            {
              push_block (puzzle, block_no, 0.5, 0  );
              push_block (puzzle, block_no, 0.5, 0  );
            }
          break;
        case 1:
          if ((block_no=get_at (puzzle, empty_x+1, empty_y))>=0)
            {
              push_block (puzzle, block_no, -0.5, 0  );
              push_block (puzzle, block_no, -0.5, 0  );
            }
          break;
        case 2:
          if ((block_no=get_at (puzzle, empty_x, empty_y-1))>=0)
            {
              push_block (puzzle, block_no, 0, 0.5);
              push_block (puzzle, block_no, 0, 0.5);
            }
          break;
        case 3:
          if ((block_no=get_at (puzzle, empty_x, empty_y+1))>=0)
            {
              push_block (puzzle, block_no, 0, -0.5);
              push_block (puzzle, block_no, 0, -0.5);
            }
          break;
        }
    }
}

static void
get_empty (Puzzle *puzzle,
             gint *col,
             gint *row)
{
  if (!col || !row)
    return;
  for (*row=0; *row < puzzle->rows; (*row)++)
     for (*col=0; *col < puzzle->cols; (*col)++)
       {
          gint item_no;
          gint found=0;
          for (item_no=0;item_no< puzzle->cols*puzzle->rows-1; item_no++)
            {
              if (puzzle->item[item_no].x == *col && puzzle->item[item_no].y == *row)
                found++;
            }
          if (!found)
            return;
       }
}

static int
get_at (Puzzle *puzzle,
        gint    col,
        gint    row)
{
  gint item_no;

  for (item_no=0;item_no< puzzle->cols*puzzle->rows-1; item_no++)
    {
      if (puzzle->item[item_no].x == col &&
          puzzle->item[item_no].y == row)
        return item_no;
    }
  return -1;
}

/* initialize a board of specified size */
static void
board_init (GtkWidget *widget)
{
  Puzzle *puzzle = PUZZLE (widget);
  gint row, col;

  if (puzzle->item)
    {
      free (puzzle->item);
    }

  puzzle->item = malloc (sizeof (PuzzleItem) * puzzle->rows * puzzle->cols);

  for (row=0;row<puzzle->rows;row++)
    {
      for (col=0;col<puzzle->cols;col++)
        {
          if (col==puzzle->cols-1 && row==puzzle->rows-1)
            {
              puzzle->item[row*puzzle->cols + col].x=col;
              puzzle->item[row*puzzle->cols + col].y=row;
              sprintf (puzzle->item[row*puzzle->cols + col].label, "-");
            }
          else 
            {
              puzzle->item[row*puzzle->cols + col].x=col;
              puzzle->item[row*puzzle->cols + col].y=row;
              sprintf (puzzle->item[row*puzzle->cols + col].label, "%i", row*puzzle->cols + col +1);
            }
        }
    }

  puzzle_shuffle (puzzle, puzzle->shuffles);
  gtk_widget_queue_draw (GTK_WIDGET (puzzle));
}

static gint
board_solved (Puzzle *puzzle)
{
  gint item_no=0;

  for (item_no=0;item_no<puzzle->rows * puzzle->cols-1;item_no++ )
    {
      PuzzleItem *item= & (puzzle->item [item_no]);

      gint row = item_no/puzzle->cols;
      gint col = item_no%puzzle->cols;

      if ( item->x != col || item->y != row)
        return 0;
    }
  return 1;
}


/* query which block is at the given mouse coordinates,
   returns -1 if no block was found
*/
static gint
query_pos (Puzzle *puzzle,
           gint    x,
           gint    y)
{
  int item_no;
  cairo_t *cr = gdk_cairo_create (GTK_WIDGET (puzzle)->window);

  if (!cr)
    {
    }
  cairo_save (cr);

  cairo_scale (cr, puzzle->ratio_x, puzzle->ratio_y);
  cairo_translate (cr, 0.5, 0.5);

  for (item_no=0;item_no<puzzle->rows*puzzle->cols-1;item_no++)
    {
      PuzzleItem *item = & (puzzle->item [item_no]);

      cairo_save (cr);
      cairo_rectangle_round (cr, item->x-0.4, item->y-0.4, 0.8, 0.8, 0.4);

      if (cairo_in_fill (cr, (x)/ puzzle->ratio_x -0.5, (y) / puzzle->ratio_y -0.5))
        {
              cairo_restore (cr);
              cairo_restore (cr);
          return item_no;
        }
      cairo_restore (cr);
    }

  cairo_restore (cr);
  return -1;
}

static gboolean
item_is_aligned (PuzzleItem *item)
{
  return (fabs(floor(item->x)-item->x)<0.01 &&
          fabs(floor(item->y)-item->y)<0.01);
}

static void
draw_item (cairo_t    *cr,
           GtkStyle   *style,
           PuzzleItem *item,
           gboolean    grabbed)
{
  gboolean aligned = item_is_aligned (item);

  cairo_rectangle_round (cr, item->x-0.36, item->y-0.36, 0.8, 0.8, 0.4);
  cairo_set_line_width (cr, 0.07);

  if (grabbed)
    {
      gdk_cairo_set_source_color (cr, &style->dark[GTK_STATE_NORMAL]);
    }
  else
    {
      if (aligned)
        gdk_cairo_set_source_color (cr, &style->dark[GTK_STATE_NORMAL]);
      else
        gdk_cairo_set_source_color (cr, &style->dark[GTK_STATE_ACTIVE]);
    }

  cairo_stroke (cr);

  cairo_rectangle_round (cr, item->x-0.4, item->y-0.4, 0.8, 0.8, 0.4);
  cairo_save (cr);

    if (grabbed)
      {
        gdk_cairo_set_source_color (cr, &style->base[GTK_STATE_SELECTED]);
      }
    else
      {
        if (aligned)
          gdk_cairo_set_source_color (cr, &style->base[GTK_STATE_NORMAL]);
        else
          gdk_cairo_set_source_color (cr, &style->base[GTK_STATE_NORMAL]);
      }

    cairo_fill (cr);
  cairo_restore (cr);

  if (grabbed)
    {
      gdk_cairo_set_source_color (cr, &style->mid[GTK_STATE_SELECTED]);
    }
  else
    {
     if (aligned)
       gdk_cairo_set_source_color (cr, &style->mid[GTK_STATE_NORMAL]);
     else
       gdk_cairo_set_source_color (cr, &style->dark[GTK_STATE_ACTIVE]);
    }
   
  cairo_set_line_width (cr, 0.05);
  cairo_stroke (cr);

  {
    cairo_text_extents_t extents;
    cairo_text_extents (cr, item->label, &extents);

    cairo_move_to (cr, item->x-extents.width/2 - extents.x_bearing,
                        item->y - extents.height/2 - extents.y_bearing);

  }
    
  if (grabbed)
    {
      gdk_cairo_set_source_color (cr, &style->text[GTK_STATE_SELECTED]);
    }
  else
    {
      if (aligned)
        gdk_cairo_set_source_color (cr, &style->text[GTK_STATE_NORMAL]);
      else
        gdk_cairo_set_source_color (cr, &style->text[GTK_STATE_NORMAL]);
    }
   cairo_show_text (cr, item->label);
}
            

/* draw the game board using the provided cairo context
*/
static void
draw_board (Puzzle  *puzzle,
            cairo_t *cr)
{
  int item_no;
  GtkStyle *style;

  style = GTK_WIDGET (puzzle)->style;

  cairo_save (cr);
    cairo_select_font_face (cr, "sans", 0, 0);
    cairo_set_font_size (cr, 0.35);
    cairo_scale (cr, puzzle->ratio_x, puzzle->ratio_y);
    cairo_translate (cr, 0.5, 0.5);

    for (item_no=0;item_no<puzzle->rows*puzzle->cols-1;item_no++)
      draw_item (cr, style, &(puzzle->item [item_no]), item_no == puzzle->grabbed);

  cairo_restore (cr);
}


/* returns which block would be pushed by shifting 'block' the given amount
   in x direction

   return: >=0 block id
            -1 none
            -2 going off board
*/
static int
who_is_pushed_x (Puzzle *puzzle,
                 int     block_no,
                 double  xdelta)
{
  int item_no;
  PuzzleItem *block= & (puzzle->item [block_no]);

  if (xdelta<0)
    {
      if (block->x+xdelta<0)
        return -2;

      for (item_no=0;item_no<puzzle->rows*puzzle->cols-1;item_no++)
        {
          PuzzleItem *item= & (puzzle->item [item_no]);
          if (block->x > item->x && block->x + xdelta - 1 <= item->x
              && item->y < block->y + 1 && item->y > block->y -1  )
            {
              return item_no;
            }
        }
    }
  else
    {
      if (block->x+xdelta>puzzle->cols-1)
        return -2;

      for (item_no=0;item_no<puzzle->rows*puzzle->cols-1;item_no++)
        {
          PuzzleItem *item= & (puzzle->item [item_no]);
          if (block->x < item->x && block->x + xdelta + 1 >= item->x
              && item->y < block->y + 1 && item->y > block->y -1  )
            {
              return item_no;
            }
        }
    }
  return -1;
}

/* returns which block would be pushed by shifting 'block' the given amount
   in y direction

   return: >=0 block id
            -1 none
            -2 going off board
*/
static int
who_is_pushed_y (Puzzle  *puzzle,
                 gint     block_no,
                 gdouble  ydelta)
{
  int item_no;
  PuzzleItem *block= & (puzzle->item [block_no]);

  if (ydelta<0)
    {
      if (block->y+ydelta<0)
          return -2;

      for (item_no=0;item_no<puzzle->rows*puzzle->cols-1;item_no++)
        {
          PuzzleItem *item= & (puzzle->item [item_no]);
          if (block->y > item->y && block->y + ydelta - 1 <= item->y
              && item->x < block->x + 1 && item->x > block->x -1  )
            {

              return item_no;
            }
        }
    }
  else
    {
      if (block->y+ydelta>puzzle->rows-1)
        return -2;

      for (item_no=0;item_no<puzzle->rows*puzzle->cols-1;item_no++)
        {
          PuzzleItem *item= & (puzzle->item [item_no]);
          if (block->y < item->y && block->y + ydelta + 1 >= item->y
              && item->x < block->x + 1 && item->x > block->x -1  )
            {
              return item_no;
            }
        }
    }
  return -1;
}

/* attempt to push a block the given delta in x and y directions */
static void
push_block (Puzzle  *puzzle,
            gint     block_no,
            gdouble  xdelta,
            gdouble  ydelta)
{
  PuzzleItem *block= & (puzzle->item [block_no]);

  gint pushed_x, pushed_y;

  if (floor (block->y)-block->y == 0 && xdelta != 0.0)
    {
      if (xdelta>=0.9)
        xdelta=0.9;
      else if (xdelta <= -0.9)
        xdelta=-0.9;

     pushed_x = who_is_pushed_x (puzzle, block_no, xdelta);

     if ( pushed_x == -1 || pushed_x == -2)
       {
         block->x += xdelta;
         if (block->x >= puzzle->cols-1)
           {
            block->x = puzzle->cols-1;
           }
         else if (block->x < 0)
           {
            block->x = 0;
           }
         return;
       }

     if (pushed_x >= 0)
       {
         push_block (puzzle, pushed_x, xdelta, 0);
         if (fabs (block->x - puzzle->item[pushed_x].x)>1.0001)
           {
             if (block->x < puzzle->item[pushed_x].x)
               block->x = puzzle->item[pushed_x].x - 1;
             else
               block->x = puzzle->item[pushed_x].x + 1;
           }
       }
    }

  if (floor (block->x)-block->x == 0)
    {
      if (ydelta>=0.9)
        ydelta=0.9;
      else if (ydelta <= -0.9)
        ydelta=-0.9;

      pushed_y = who_is_pushed_y (puzzle, block_no, ydelta);

      if (pushed_y == -1 || pushed_y == -2)
        {
          block->y += ydelta;
          if (block->y >= puzzle->rows-1)
            {
              block->y = puzzle->rows-1;
            }
          else if (block->y < 0)
            {
              block->y = 0;
            }
          return;
        }

      if (pushed_y >= 0)
        {
          push_block (puzzle, pushed_y, 0, ydelta);
          if (fabs (block->y - puzzle->item[pushed_y].y)>1.0001)
            {
              if (block->y < puzzle->item[pushed_y].y)
                block->y = puzzle->item[pushed_y].y - 1;
              else
                block->y = puzzle->item[pushed_y].y + 1;
            }
        }
    }
  return;
}
