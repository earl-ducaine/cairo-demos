/*******************************************************************************
 **3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
 **      10        20        30        40        50        60        70        80
 **
 ** program:
 **    wobbly-zini
 **
 ** author:
 **    Mirco "MacSlow" Mueller <macslow@bangang.de>
 **
 ** created:
 **    22.1.2006
 **
 ** last change:
 **    22.1.2006
 **
 ** notes:
 **    This is an ultra-quick hack for fun to get a bit of "wow the crowds". It's
 **    just a "port" of one of my <canvas>-tag hacks, which I also did as a
 **    sample for cairo-in-motion, to become a freely floating eye-candy gimmick
 **    on the desktop. I place this program under the "GNU General Public
 **    License". If you don't know what that means take a look a here...
 **
 **        http://www.gnu.org/licenses/licenses.html#GPL
 **
 ** todo:
 **    - nothing really, it's just an utter hack
 **
 *******************************************************************************/

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <cairo/cairo.h>
#include <math.h>
#include <sys/time.h>
#include <strings.h>

#define WIN_WIDTH 400
#define WIN_HEIGHT 400
#define MS_INTERVAL 10

cairo_t* g_cairo_context_ptr;
struct timeval	g_timeValue;

void wobbly_zini_render(cairo_t* cairo_ptr, int width, int height);

static gboolean time_handler(gpointer* widget_ptr)
{
    gtk_widget_queue_draw((GtkWidget*)widget_ptr);
    return TRUE;
}

static gboolean on_alpha_window_draw(GtkWidget* widget_ptr,
				      cairo_t* cairo_ptr,
				      gpointer user_data)
{
    static gint iWidth;
    static gint iHeight;

    // cairo_ptr = gdk_cairo_create(pWidget->window);
    GdkWindow* window_ptr = gtk_widget_get_window(widget_ptr);
    // GdkDrawingContext* context_ptr = gdk_window_begin_draw_frame(window_ptr, expose_ptr->expose.region);
    // gdk_drawing_context_get_cairo_context(context_ptr);

    cairo_set_operator(cairo_ptr, CAIRO_OPERATOR_SOURCE);
    gtk_window_get_size(GTK_WINDOW(widget_ptr), &iWidth, &iHeight);
    wobbly_zini_render(cairo_ptr, iWidth, iHeight);
    // cairo_destroy(cairo_ptr);

    // gdk_window_end_draw_frame(pWidget->window, expose_ptr->expose.region);

    return FALSE;
}

static void on_alpha_screen_changed(GtkWidget*	pWidget,
				     GdkScreen*	pOldScreen,
				     GtkWidget*	pLabel)
{
    GdkScreen* pScreen = gtk_widget_get_screen(pWidget);
    GdkVisual* pColormap = gdk_screen_get_rgba_visual(pScreen);

    if (!pColormap)
    {
	pColormap = gdk_screen_get_rgba_visual(pScreen);
	printf("screen does NOT support alpha\n");
    }
    else
	printf("screen supports alpha!\n");

    gtk_widget_set_visual(pWidget, pColormap);
}

gboolean on_key_press(GtkWidget	*	pWidget,
		       GdkEventKey*	pKey,
		       gpointer		userData)
{
    if (pKey->type == GDK_KEY_PRESS)
    {
	switch(pKey->keyval)
	{
	case GDK_KEY_Escape :
	    gtk_main_quit();
	    break;
	}
    }

    return FALSE;
}

gboolean on_button_press(GtkWidget* pWidget,
			  GdkEventButton* pButton,
			  GdkWindowEdge edge)
{
    if (pButton->type == GDK_BUTTON_PRESS)
    {
	if (pButton->button == 1)
	    gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(pWidget)),
					pButton->button,
					pButton->x_root,
					pButton->y_root,
					pButton->time);
	if (pButton->button == 2)
	    gtk_window_begin_resize_drag(GTK_WINDOW(gtk_widget_get_toplevel(pWidget)),
					  edge,
					  pButton->button,
					  pButton->x_root,
					  pButton->y_root,
					  pButton->time);
    }

    return TRUE;
}

void wobbly_zini_render(cairo_t* cairo_ptr, int width, int height)
{
    double fLength = 1.0f / 25.0f;
    double fY;
    int i;
    unsigned long ulMilliSeconds;

    cairo_save(cairo_ptr);
    cairo_scale(cairo_ptr,(double) width / 1.0f,(double) height / 1.0f);

    gettimeofday(&g_timeValue, NULL);
    ulMilliSeconds = g_timeValue.tv_usec / 1000.0f;

    /* "clear" the background of the window to fully transparent */
    cairo_set_source_rgba(cairo_ptr, 0.0f, 0.0f, 0.0f, 0.0f);
    cairo_paint(cairo_ptr);

    cairo_set_operator(cairo_ptr, CAIRO_OPERATOR_OVER);
    cairo_set_line_cap(cairo_ptr, CAIRO_LINE_CAP_ROUND);
    for(i = 0; i < 60; i++)
    {
	cairo_save(cairo_ptr);
	cairo_set_line_width(cairo_ptr, fLength);
	cairo_translate(cairo_ptr, 0.5f, 0.5f);
	cairo_rotate(cairo_ptr, M_PI/180.0f *(ulMilliSeconds + 10.0f*i) * 0.36f);
	fY = 0.33f + 0.0825f * sin((ulMilliSeconds + 10.0f*i)/1000 * 10 * M_PI);
	cairo_translate(cairo_ptr, 0, fY);
	cairo_rotate(cairo_ptr, M_PI/180.0f * 6.0f * i);
	cairo_set_source_rgba(cairo_ptr, 1.0f, 0.5f, 0.0f, i*0.01f);
	cairo_move_to(cairo_ptr, -fLength, 0);
	cairo_line_to(cairo_ptr, fLength, 0);
	cairo_stroke(cairo_ptr);
	cairo_restore(cairo_ptr);
    }
}

int main(int argc, char **argv)
{
    GtkWidget* pWindow = NULL;
    GdkGeometry hints;

    gtk_init(&argc, &argv);

    pWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_decorated(GTK_WINDOW(pWindow), FALSE);
    gtk_window_set_resizable(GTK_WINDOW(pWindow), TRUE);
    gtk_window_set_position(GTK_WINDOW(pWindow), GTK_WIN_POS_CENTER_ALWAYS);
    gtk_widget_set_app_paintable(pWindow, TRUE);
    gtk_window_set_icon_from_file(GTK_WINDOW(pWindow), "./wobbly-zini-icon.png", NULL);
    gtk_window_set_title(GTK_WINDOW(pWindow), "MacSlow's Wobbly-Zini");
    gtk_window_set_default_size(GTK_WINDOW(pWindow), WIN_WIDTH, WIN_HEIGHT);

    hints.min_width = 64;
    hints.min_height = 64;
    hints.max_width = 512;
    hints.max_height = 512;

    gtk_window_set_geometry_hints(GTK_WINDOW(pWindow),
				   pWindow,
				   &hints,
				   GDK_HINT_MIN_SIZE |
				   GDK_HINT_MAX_SIZE);

    on_alpha_screen_changed(pWindow, NULL, NULL);

    /* that's needed here because a top-level GtkWindow does not listen to
     * "button-press-events" by default */
    gtk_widget_add_events(pWindow, GDK_BUTTON_PRESS_MASK);

    g_signal_connect(G_OBJECT(pWindow),
		      "expose-event",
		      G_CALLBACK(on_alpha_window_draw),
		      NULL);

    g_signal_connect(G_OBJECT(pWindow),
		      "key-press-event",
		      G_CALLBACK(on_key_press),
		      NULL);

    g_signal_connect(G_OBJECT(pWindow),
		      "button-press-event",
		      G_CALLBACK(on_button_press),
		      NULL);

    if (!gtk_widget_get_visible(pWindow))
	gtk_widget_show_all(pWindow);

    g_timeout_add(MS_INTERVAL, (GSourceFunc)time_handler, pWindow);

    // cairo_t* cairo_ptr = gdk_cairo_create(pWindow->window);
    // cairo_t* cairo_ptr = gdk_window_begin_draw_frame(pWindow->window);

    bzero(&g_timeValue, sizeof(g_timeValue));

    gtk_main();

    return 0;
}
