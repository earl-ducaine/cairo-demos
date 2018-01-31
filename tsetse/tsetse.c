/* tsetse - A solitaire game of pattern recognition.
 *
 * Copyright © 2005 Carl D. Worth <cworth@cworth.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#include <cairo.h>
#include <cairo-xlib.h>

#include <assert.h>

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

#define SHRINK(cr, x) cairo_translate (cr, (1-(x))/2.0, (1-(x))/2.0); cairo_scale (cr, (x), (x))

#define NUM_NUMBERS 3
typedef enum { COLOR_RED, COLOR_GREEN, COLOR_PURPLE } color_t;
#define NUM_COLORS (COLOR_PURPLE + 1)
typedef enum { SHADING_EMPTY, SHADING_STRIPED, SHADING_SOLID } shading_t;
#define NUM_SHADINGS (SHADING_SOLID + 1)
typedef enum { SHAPE_DIAMOND, SHAPE_OVAL, SHAPE_SQUIGGLE } shape_t;
#define NUM_SHAPES (SHAPE_SQUIGGLE + 1)

typedef struct card {
    int number;
    color_t color;
    shading_t shading;
    shape_t shape;
} card_t;

#define DECK_MAX_CARDS (NUM_NUMBERS * NUM_COLORS * NUM_SHADINGS * NUM_SHAPES)
typedef struct deck {
    int num_cards;
    card_t cards[DECK_MAX_CARDS];
} deck_t;

typedef struct slot {
    int has_card;
    card_t card;
    int selected;
} slot_t;

#define BOARD_COLS 3
#define BOARD_ROWS 4
#define BOARD_MAX_SLOTS (BOARD_COLS * BOARD_ROWS)
typedef struct board {
    int num_slots;
    slot_t slots[BOARD_MAX_SLOTS];
    int needs_deal;
    int sets_possible;
    int display_sets_possible;
} board_t;

typedef struct win {
    Display *dpy;
    int scr;
    Window win;
    GC gc;
    Pixmap pix;
    cairo_surface_t *surface;
    int width, height;
    long event_mask;

    int needs_refresh;

    int click;
    int active;
    int display_sets_possible;

    deck_t deck;
    board_t board;
} win_t;

typedef struct callback_doc {
    void *callback;
    const char *doc;
} callback_doc_t;

typedef int (*key_callback_t)(win_t *win);

typedef struct key_binding
{
    const char *key;
    int is_alias;
    KeyCode keycode;
    key_callback_t callback;
} key_binding_t;

static void win_init(win_t *win, Display *dpy);
static void win_deinit(win_t *win);
static void win_refresh(win_t *win);
static void win_select_events(win_t *win);
static void win_handle_events(win_t *win);
static void win_print_help(win_t *win);

static int new_game_cb(win_t *win);
static int reshuffle_cb(win_t *win);
static int toggle_display_sets_possible_cb(win_t *win);
static int quit_cb(win_t *win);

static void
board_count_sets_possible (board_t *board);

static const callback_doc_t callback_doc[] = {
    { new_game_cb,      "New game" },
    { reshuffle_cb,	"Return dealt cards to deck and reshuffle" },
    { toggle_display_sets_possible_cb, "Toggle display of possible sets" },
    { quit_cb,		"Exit the program" }
};

static key_binding_t key_binding[] = {
    /* Keysym, Alias, Keycode, callback */
    { "N",      0, 0, new_game_cb },
    { "R",      0, 0, reshuffle_cb },
    { "space",  0, 0, toggle_display_sets_possible_cb },
    { "Q",	0, 0, quit_cb }
};

int
main(void)
{
    win_t win;

    Display *dpy = XOpenDisplay(0);

    srand (time(0));

    if (dpy == NULL) {
	fprintf(stderr, "Failed to open display: %s\n", XDisplayName(0));
	return 1;
    }

    win_init(&win, dpy);

    win_print_help(&win);

    win_handle_events(&win);

    win_deinit(&win);

    XCloseDisplay(dpy);

    return 0;
}

static void
draw_symbol (cairo_t *cr, color_t color, shading_t shading, shape_t shape)
{
    cairo_pattern_t *stripes;

    cairo_save (cr);

    SHRINK (cr, .9);

    stripes = cairo_pattern_create_linear (0, 0, .1, .1);
    cairo_pattern_add_color_stop_rgba (stripes, 0.0, 0, 0, 0, 0);
    cairo_pattern_set_extend (stripes, CAIRO_EXTEND_REPEAT);

    switch (color) {
    case COLOR_RED:
	cairo_set_source_rgb (cr, .8, 0, 0);
	cairo_pattern_add_color_stop_rgba (stripes, 0.3, .8, 0, 0, 1.0);
	cairo_pattern_add_color_stop_rgba (stripes, 0.7, .8, 0, 0, 1.0);
	break;
    case COLOR_GREEN:
	cairo_set_source_rgb (cr, 0, .6, 0);
	cairo_pattern_add_color_stop_rgba (stripes, 0.3, 0, .6, 0, 1.0);
	cairo_pattern_add_color_stop_rgba (stripes, 0.7, 0, .6, 0, 1.0);
	break;
    case COLOR_PURPLE:
	cairo_set_source_rgb (cr, .6, 0, .7);
	cairo_pattern_add_color_stop_rgba (stripes, 0.3, .6, 0, .7, 1.0);
	cairo_pattern_add_color_stop_rgba (stripes, 0.7, .6, 0, .7, 1.0);
	break;
    }

    cairo_pattern_add_color_stop_rgba (stripes, 1.0, 0, 0, 0, 0);

    switch (shape) {
    case SHAPE_DIAMOND:
	cairo_move_to (cr, 0.5, 0);
	cairo_line_to (cr, 1.0, 0.5);
	cairo_line_to (cr, 0.5, 1.0);
	cairo_line_to (cr, 0, 0.5);
	cairo_close_path (cr);
	break;
    case SHAPE_OVAL:
	cairo_new_path (cr);
	cairo_arc (cr, 0.5, 0.5, 0.5, 0, 2 * M_PI);
	cairo_close_path (cr);
	break;
    case SHAPE_SQUIGGLE:
#define SQF .75
	cairo_move_to (cr, .75, 0);
	cairo_curve_to (cr,
			.75+SQF, SQF/3.0,
			.75-SQF, 1-(SQF/3.0),
			.75, 1.0);
	cairo_line_to (cr, .25, 1.0);
	cairo_curve_to (cr,
			.25-SQF, 1-(SQF/3.0),
			.25+SQF, SQF/3.0,
			.25, 0.0);
	cairo_close_path (cr);
	break;
    }

#define LINE_WIDTH 0.015
    switch (shading) {
    case SHADING_EMPTY:
	cairo_scale (cr, 3.0, 1.0);
	cairo_set_line_width (cr, LINE_WIDTH);
	cairo_stroke (cr);
	break;
    case SHADING_STRIPED:
	{
	    cairo_set_source (cr, stripes);
	    cairo_fill (cr);
/* XXX: This would probably work OK, but clipping is too slow right now. */	
#if 0
	cairo_clip (cr);
	cairo_save (cr);
	cairo_scale (cr, 3.0, 1.0);
	cairo_set_line_width (cr, 2*LINE_WIDTH);
	cairo_stroke (cr);
	cairo_restore (cr);
	cairo_new_path (cr);
#define NUM_STRIPES 10
	for (i=0; i<NUM_STRIPES; i++) {
	    cairo_move_to (cr, 0, .1+2*(double) i/NUM_STRIPES);
	    cairo_line_to (cr, .1+2*(double) i/NUM_STRIPES, 0);
	    cairo_save (cr);
	    cairo_scale (cr, 3.0, 1.0);
	    cairo_set_line_width (cr, LINE_WIDTH);
	    cairo_stroke (cr);
	    cairo_restore (cr);
	}
#endif
	}
	break;
    case SHADING_SOLID:
	cairo_fill (cr);
	break;
    }

    cairo_pattern_destroy (stripes);

    cairo_restore (cr);
}

static void
draw_card (cairo_t *cr, card_t *card, int highlight)
{
    static const double border_width = .02;
    int i;

    cairo_save (cr);

    SHRINK (cr, .9);

    cairo_rectangle (cr, 0, 0, 1.0, 1.0);
    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_set_line_width (cr, border_width);
    cairo_stroke (cr);
 
    cairo_save (cr);
    {
	SHRINK (cr, 1-border_width);

	for (i=0; i < card->number; i++) {
	    cairo_save (cr);
	    cairo_scale (cr, 1.0 / 3, 1.0);
	    cairo_translate (cr, (3-card->number)/2.0 + i, 0);
	    draw_symbol (cr, card->color, card->shading, card->shape);
	    cairo_restore (cr);
	}
    }
    cairo_restore (cr);

    if (highlight) {
	cairo_rectangle (cr, 0, 0, 1.0, 1.0);
	cairo_set_source_rgba (cr, 0, 0, 1, 0.25);
	cairo_fill (cr);
    }
	
    cairo_restore (cr);
}

static int
is_set (card_t *a, card_t *b, card_t *c)
{
    return (
	    (
	     (a->number == b->number && b->number == c->number && a->number == c->number)
	     ||
	     (a->number != b->number && b->number != c->number && a->number != c->number)
	    )
	    &&
	    (
	     (a->color == b->color && b->color == c->color && a->color == c->color)
	     ||
	     (a->color != b->color && b->color != c->color && a->color != c->color)
	    )
	    &&
	    (
	     (a->shading == b->shading && b->shading == c->shading && a->shading == c->shading)
	     ||
	     (a->shading != b->shading && b->shading != c->shading && a->shading != c->shading)
	    )
	    &&
	    (
	     (a->shape == b->shape && b->shape == c->shape && a->shape == c->shape)
	     ||
	     (a->shape != b->shape && b->shape != c->shape && a->shape != c->shape)
	    )
	   );
}

static int
check_selected_for_set (board_t *board)
{
    int i, num_selected;
    slot_t *slots[3];

    num_selected = 0;
    for (i=0; i < board->num_slots; i++) {
	if (board->slots[i].selected) {
	    if (num_selected >= 3)
		return 0;
	    slots[num_selected++] = &board->slots[i];
	}
    }

    if (num_selected !=3)
	return 0;

    if (! is_set (&slots[0]->card, &slots[1]->card, &slots[2]->card))
	return 0;

    for (i=0; i < 3; i++) {
	slots[i]->selected = 0;
	slots[i]->has_card = 0;
    }

    board_count_sets_possible (board);

    board->needs_deal = 1;

    return 1;
}

static void
board_click (board_t *board, double x, double y)
{
    int col = (int) (x * BOARD_COLS);
    int row = (int) (y * BOARD_ROWS);

    int slot = row * BOARD_COLS + col;
    board->slots[slot].selected = ! board->slots[slot].selected;

    check_selected_for_set (board);
}

static void
draw_board (cairo_t *cr, board_t *board, int width, int height)
{
    int i;

    cairo_save (cr);

    cairo_scale (cr, width, height);

    for (i=0; i < board->num_slots; i++)
	if (board->slots[i].has_card) {
	    cairo_save (cr);
	    cairo_translate (cr,
			     1.0/BOARD_COLS * (i % BOARD_COLS),
			     1.0/BOARD_ROWS * (i / BOARD_COLS));
	    cairo_scale (cr, 1.0/BOARD_COLS, 1.0/BOARD_ROWS);
	    draw_card (cr, &board->slots[i].card, board->slots[i].selected);
	    cairo_restore (cr);
	}

    cairo_restore (cr);
}

static void
win_refresh(win_t *win)
{
    cairo_t *cr;
    cairo_status_t status;

    cr = cairo_create(win->surface);

    cairo_rectangle (cr, 0, 0, win->width, win->height);
    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_fill (cr);

    draw_board (cr, &win->board, win->width, win->height);

    if (win->display_sets_possible) {
	char sets_possible[3];
	cairo_text_extents_t extents;

	snprintf (sets_possible, 3, "%d", win->board.sets_possible);
	sets_possible[2] = '\0';
	
	cairo_save (cr);
	{
	    cairo_select_font_face (cr, "sans", 0, 0);
	    cairo_set_font_size (cr, win->height / 1.2);
	    cairo_move_to (cr, 0, 0);
	    cairo_text_extents (cr, sets_possible, &extents);
	    cairo_move_to (cr,
			   win->width/2 - (extents.x_bearing + extents.width/2),
			   win->height/2 - (extents.y_bearing + extents.height/2));
	    cairo_set_source_rgba (cr, 0, 0, .5, 0.75);
	    cairo_show_text (cr, sets_possible);
	}
	cairo_restore (cr);
    }

    status = cairo_status(cr);
    if (status) {
	fprintf(stderr, "Cairo is unhappy: %s\n",
		cairo_status_to_string(cairo_status(cr)));
    }

    cairo_destroy(cr);

    XCopyArea(win->dpy, win->pix, win->win, win->gc,
	      0, 0, win->width, win->height,
	      0, 0);
}

static void
deck_shuffle (deck_t *deck)
{
    int i, r;
    card_t tmp;

    assert (deck->num_cards <= DECK_MAX_CARDS);

    for (i=deck->num_cards - 1; i>=0; i--) {
	r = (int) i * (rand() / (RAND_MAX + 1.0));
	assert (r >= 0);
	assert (r <= i);
	tmp = deck->cards[i];
	deck->cards[i] = deck->cards[r];
	deck->cards[r] = tmp;
    }
}

static void
deck_init (deck_t *deck)
{
    int card;
    int number;
    color_t color;
    shading_t shading;
    shape_t shape;

    card = 0;
    for (number = 1; number <= NUM_NUMBERS; number++)
	for (color = 0; color < NUM_COLORS; color++)
	    for (shading = 0; shading < NUM_SHADINGS; shading++)
		for (shape = 0; shape < NUM_SHAPES; shape++) {
		    deck->cards[card].number = number;
		    deck->cards[card].color = color;
		    deck->cards[card].shading = shading;
		    deck->cards[card].shape = shape;
		    card++;
		}
    deck->num_cards = card;

    deck_shuffle (deck);
}

static void
board_count_sets_possible (board_t *board)
{
    int i, j, k;
    int sets_possible = 0;

    for (i = 0; i < board->num_slots; i++) {
	if (! board->slots[i].has_card)
	    continue;
	for (j = i+1; j < board->num_slots; j++) {
	    if (! board->slots[j].has_card)
		continue;
	    for (k = j+1; k < board->num_slots; k++) {
		if (! board->slots[k].has_card)
		    continue;
		if (is_set (&board->slots[i].card,
			    &board->slots[j].card,
			    &board->slots[k].card))
		    sets_possible++;
	    }
	}
    }

    board->sets_possible = sets_possible;
}

static void
board_init (board_t *board)
{
    int i;
    board->num_slots = BOARD_MAX_SLOTS;
    for (i=0; i < board->num_slots; i++) {
	board->slots[i].has_card = 0;
	board->slots[i].selected = 0;
    }
    board->needs_deal = 0;

    board_count_sets_possible (board);
}

static void
deal (deck_t *deck, board_t *board)
{
    int i;

    for (i=0; i < board->num_slots; i++)
	if (! board->slots[i].has_card) {
	    if (deck->num_cards > 0) {
		board->slots[i].card = deck->cards[deck->num_cards-- -1];
		board->slots[i].has_card = 1;
	    }
	}

    board_count_sets_possible (board);

    board->needs_deal = 0;
}

/* Begin a new game */
static void
new_game (deck_t *deck, board_t *board)
{
    deck_init (deck);
    board_init (board);
    deal (deck, board);
}

/* Return the dealt cards to the deck, reshuffle, and deal again. */
static int
reshuffle (deck_t *deck, board_t *board)
{
    int i;

    if (board->sets_possible) {
	return 0;
    }

    for (i=0; i < board->num_slots; i++) {
	if (board->slots[i].has_card) {
	    deck->cards[++deck->num_cards - 1] = board->slots[i].card;
	    board->slots[i].has_card = 0;
	    board->slots[i].selected = 0;
	}
    }

    deck_shuffle (deck);
    deal (deck, board);

    return 1;
}

static void
win_init(win_t *win, Display *dpy)
{
    unsigned int i;
    Window root;
    XGCValues gcv;

    new_game (&win->deck, &win->board);

    win->dpy = dpy;
    win->width = 600;
    win->height = 600;

    root = DefaultRootWindow(dpy);
    win->scr = DefaultScreen(dpy);

    win->win = XCreateSimpleWindow(dpy, root, 0, 0,
				   win->width, win->height, 0,
				   WhitePixel(dpy, win->scr), WhitePixel(dpy, win->scr));

    win->pix = XCreatePixmap(dpy, win->win, win->width, win->height, DefaultDepth (dpy, win->scr));
    win->surface = cairo_xlib_surface_create (dpy, win->pix, DefaultVisual (dpy, win->scr), win->width, win->height);

    gcv.foreground = WhitePixel(dpy, win->scr);
    win->gc = XCreateGC(dpy, win->pix, GCForeground, &gcv);
    XFillRectangle(dpy, win->pix, win->gc, 0, 0, win->width, win->height);

    for (i=0; i < ARRAY_SIZE(key_binding); i++) {
	KeySym keysym;
	keysym = XStringToKeysym(key_binding[i].key);
	if (keysym == NoSymbol)
	    fprintf(stderr, "ERROR: No keysym for \"%s\"\n", key_binding[i].key);
	else
	    key_binding[i].keycode = XKeysymToKeycode(dpy, keysym);
    }

    win->active = 0;
    win->click = 0;
    win->display_sets_possible = 0;

    win_refresh(win);
    win->needs_refresh = 0;

    win_select_events(win);

    XMapWindow(dpy, win->win);
}

static void
win_deinit(win_t *win)
{
    XFreeGC(win->dpy, win->gc);
    XFreePixmap(win->dpy, win->pix);
    cairo_surface_destroy (win->surface);
    XDestroyWindow(win->dpy, win->win);
}

static void
win_select_events(win_t *win)
{
    win->event_mask = 
	  ButtonPressMask
	| ButtonReleaseMask
	| KeyPressMask
	| StructureNotifyMask
	| ExposureMask;
    XSelectInput(win->dpy, win->win, win->event_mask);

}

static const char *
get_callback_doc(void *callback)
{
    unsigned int i;

    for (i=0; i < ARRAY_SIZE(callback_doc); i++)
	if (callback_doc[i].callback == callback)
	    return callback_doc[i].doc;

    return "<undocumented function>";
}

static void
win_print_help(win_t *win)
{
    unsigned int i;

    printf("Welcome to the game of tsetse\n");
    printf("Click on three cards that form a set. Useful keys:\n\n");

    for (i=0; i < ARRAY_SIZE(key_binding); i++)
	if (! key_binding[i].is_alias)
	    printf("%s:\t%s\n",
		   key_binding[i].key,
		   get_callback_doc(key_binding[i].callback));
}

static void
win_handle_button_press(win_t *win, XButtonEvent *bev)
{
    win->click = 1;

    board_click (&win->board,
		 (double) bev->x / win->width,
		 (double) bev->y / win->height);

    if (win->board.needs_deal)
	deal (&win->deck, &win->board);
 
    win->needs_refresh = 1;
}

static int
win_handle_key_press(win_t *win, XKeyEvent *kev)
{
    unsigned int i;

    for (i=0; i < ARRAY_SIZE(key_binding); i++)
	if (key_binding[i].keycode == kev->keycode)
	    return (key_binding[i].callback)(win);
	
    return 0;
}

static void
win_grow_pixmap(win_t *win)
{
    Pixmap new;

    new = XCreatePixmap(win->dpy, win->win, win->width, win->height, DefaultDepth (win->dpy, win->scr));
    XFillRectangle(win->dpy, new, win->gc, 0, 0, win->width, win->height);
    XCopyArea(win->dpy, win->pix, new, win->gc, 0, 0, win->width, win->height, 0, 0);
    XFreePixmap(win->dpy, win->pix);
    cairo_surface_destroy (win->surface);
    win->pix = new;
    win->surface = cairo_xlib_surface_create (win->dpy, win->pix,
					      DefaultVisual (win->dpy, win->scr),
					      win->width, win->height);
}

static void
win_handle_configure(win_t *win, XConfigureEvent *cev)
{
    int has_grown = 0;

    if (cev->width > win->width || cev->height > win->height) {
	has_grown = 1;
    }

    win->width = cev->width;
    win->height = cev->height;

    if (has_grown) {
	win_grow_pixmap(win);
    }

    win->needs_refresh = 1;
}

static void
win_handle_expose(win_t *win, XExposeEvent *eev)
{
    XCopyArea(win->dpy, win->pix, win->win, win->gc,
	      eev->x, eev->y, eev->width, eev->height,
	      eev->x, eev->y);
}

static void
win_handle_events(win_t *win)
{
    int done;
    XEvent xev;

    while (1) {

	if (!XPending(win->dpy) && win->needs_refresh) {
	    win_refresh(win);
	    win->needs_refresh = 0;
	}

	XNextEvent(win->dpy, &xev);

	switch(xev.type) {
	case ButtonPress:
	    win_handle_button_press(win, &xev.xbutton);
	    break;
/*
	case MotionNotify:
	    win_handle_motion(win, &xev.xmotion);
	    break;
*/
	case ButtonRelease:
	    win->click = 0;
	    win->active = 0;
	    break;
	case KeyPress:
	    done = win_handle_key_press(win, &xev.xkey);
	    if (done)
		return;
	    break;
	case ConfigureNotify:
	    win_handle_configure(win, &xev.xconfigure);
	    break;
	case Expose:
	    win_handle_expose(win, &xev.xexpose);
	    break;
	}
    }
}


/* Callbacks */

static int
new_game_cb (win_t *win)
{
    new_game (&win->deck, &win->board);
    win->needs_refresh = 1;

    return 0;
}

static int
reshuffle_cb (win_t *win)
{
    if (reshuffle (&win->deck, &win->board))
	win->display_sets_possible = 0;
    else
	win->display_sets_possible = 1;

    win->needs_refresh = 1;

    return 0;
}

static int
toggle_display_sets_possible_cb (win_t *win)
{
    win->display_sets_possible = !win->display_sets_possible;

    win->needs_refresh = 1;

    return 0;
}

static int
quit_cb (win_t *win)
{
    return 1;
}
