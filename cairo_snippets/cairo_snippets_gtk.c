/* GtkCairo ui for cairo_snippets,
 * (c) Øyvind Kolås 2004, placed in the Public Domain
 */

#include "snippets.h"
#include <gtkcairo.h>
#include <gtk/gtk.h>
#include <stdio.h>

static GtkWidget     *gtkcairo;
static GtkTextBuffer *source_buffer;
int                   current_snippet=0;

static void
paint (GtkWidget *widget,
        cairo_t   *cr,
        void      *data)
{
    gint width = widget->allocation.width;
    gint height = widget->allocation.height;

    cairo_save (cr);
      snippet_do (cr, current_snippet, width, height);
    cairo_restore (cr);
}

static void
selection_cb (GtkTreeSelection *selection,
              GtkTreeModel     *model)
{
  GtkTreeIter iter;
  GValue value = {0, };

  if (!gtk_tree_selection_get_selected (selection, NULL, &iter))
     return;

  gtk_tree_model_get_value (model, &iter, 0, &value);

  if (g_value_get_string (&value)){
    current_snippet = snippet_name2no (g_value_get_string (&value));
    gtk_text_buffer_set_text (source_buffer, snippet_source [current_snippet], -1);
    gtk_widget_queue_draw (gtkcairo);
  }
  g_value_unset (&value);
}

static GtkWidget *
create_snippet_list (void)
{
    GtkListStore      *model;
    GtkWidget         *tree_view;
    GtkTreeSelection  *selection;
    GtkCellRenderer   *renderer;
    GtkTreeViewColumn *col;
    GtkTreeIter        iter;
    GtkWidget         *scroll_window;
    gint               i;

    model = gtk_list_store_new (1, G_TYPE_STRING);
    tree_view = gtk_tree_view_new_with_model (GTK_TREE_MODEL (model));
    selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (tree_view));

    gtk_tree_selection_set_mode (GTK_TREE_SELECTION (selection), GTK_SELECTION_BROWSE);
    gtk_widget_set_size_request (tree_view, 200, -1);

    for (i=0;i<snippet_count;i++) {
        gtk_list_store_append (GTK_LIST_STORE (model), &iter);
        gtk_list_store_set (GTK_LIST_STORE (model), &iter, 0, snippet_name[i], -1);
    }

    renderer = gtk_cell_renderer_text_new ();
    col = gtk_tree_view_column_new ();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_add_attribute (col, renderer, "text", 0);
    gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view), col);

    g_object_set (G_OBJECT (tree_view), "headers-visible", FALSE,
                                        "search-column", 0,
                                        "rules-hint", FALSE,
                                        NULL);

    g_signal_connect (selection, "changed", G_CALLBACK (selection_cb), model);

    scroll_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll_window),
                       GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_container_add (GTK_CONTAINER (scroll_window), tree_view);

    return scroll_window;
}

static GtkWidget *
create_source_view (void)
{
    GtkWidget *scroll_window;
    GtkWidget *text_view;

    text_view = gtk_text_view_new ();
    gtk_widget_set_size_request (text_view, -1, 200);

    source_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text_view));

    scroll_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll_window),
                       GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_container_add (GTK_CONTAINER (scroll_window), text_view);

    return scroll_window;
}

int
main (int argc, char *argv[])
{
    GtkWidget *win, *hpaned, *vpaned, *list, *source_view;

    gtk_init (&argc, &argv);

    win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (win), "GtkCairo snippets");
    g_signal_connect (G_OBJECT (win), "delete-event",
                      G_CALLBACK (gtk_main_quit), NULL);

    hpaned = gtk_hpaned_new ();
    vpaned = gtk_vpaned_new ();

    gtkcairo = gtk_cairo_new ();
    gtk_widget_set_usize (GTK_WIDGET (gtkcairo), 256, 256);

    g_signal_connect (G_OBJECT (gtkcairo), "paint",
                      G_CALLBACK (paint), NULL);

    source_view = create_source_view ();
    list = create_snippet_list ();

    gtk_paned_pack1 (GTK_PANED (hpaned), list, FALSE, TRUE);
    gtk_paned_pack2 (GTK_PANED (hpaned), gtkcairo, TRUE, FALSE);

    gtk_paned_pack1 (GTK_PANED (vpaned), source_view, FALSE, TRUE);
    gtk_paned_pack2 (GTK_PANED (vpaned), hpaned, TRUE, FALSE);

    gtk_container_add (GTK_CONTAINER (win), vpaned);

    gtk_widget_show_all (win);
    gtk_main ();
    return 0;
}
