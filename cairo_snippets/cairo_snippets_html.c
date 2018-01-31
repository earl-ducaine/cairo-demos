/* html index generator for cairo_snippets
 * (c) Øyvind Kolås 2004, placed in the public domain
 */

#include "snippets.h"

#define TEMPLATE_DIR "html/"
#define OUTPUT_DIR "snippets/"

void
write_index (void);

void
write_all (void);

void
write_html (int snippet_no);

static void
embed_file (FILE *file,
            const char *filename);

int
main (int argc, char **argv)
{
        int i;
        for (i=0;i<snippet_count;i++)
                write_html (i);
        write_index ();
        write_all ();
        return 0;
}

static void
do_index(FILE *file, int active)
{
    int i;
    fprintf (file, "<div id='snippet_list'>\n");

/*    if (active==-1)
       fprintf (file, "all&nbsp;&lt;---<br />\n");
    else
       fprintf (file, "<a href='snippets.html'>all</a><br />\n");
*/
    for (i=0;i<snippet_count;i++) {
        if (i==active)
           fprintf (file, "<div id='snippet_list_entry_selected'><a href='%s.html'>%s\n</a></div>", snippet_name[i], snippet_name[i]);
        else
           fprintf (file, "<div id='snippet_list_entry'><a href='%s.html'>%s</a>\n</div>",
                        snippet_name[i], snippet_name[i]);
    }

    fprintf (file, "</div>\n");
}

void
write_index(void)
{
    FILE *file= fopen (OUTPUT_DIR "index.html", "w");

    embed_file (file, TEMPLATE_DIR "header.html_template");
    do_index (file, -2);
    embed_file (file, TEMPLATE_DIR "index.html_template");
    embed_file (file, TEMPLATE_DIR "footer.html_template");
    fclose (file);
}

void
write_all(void)
{
    int i=0;
    FILE *file= fopen (OUTPUT_DIR "snippets.html", "w");

    embed_file (file, TEMPLATE_DIR "header.html_template");
    fprintf (file, "<p>Renderings done using the cairo api, and their corresponding drawing instructions.</p>\n");
    fprintf (file, "<table>\n");
    fprintf (file, "<div id='snippets'>");
    for (i=0;i<snippet_count;i++) {
            const char *name=snippet_name[i];
            const char *c=&snippet_source[i][0];

            fprintf (file, "<div id='snippet'>\n");
            fprintf (file, "<div id='snippet_title'><a href='%s.html'><b>%s</b></a><a name='%s'></a></div>\n", name,name,name);
            fprintf (file, "<div id='snippet_image'><img src='%s.png'/></div>\n", name);
            fprintf (file, "<div id='snippet_source'><pre>");

            while (*c){
                    switch (*c){
                        case '<':
                            fprintf (file, "&lt;");
                            break;
                        case '>':
                            fprintf (file, "&gt;");
                            break;
                        case '&':
                            fprintf (file, "&amp;");
                            break;
                        default:
                            fprintf (file, "%c", *c);
                    }
                    c++;
            }

            fprintf (file, "       </pre></div>\n");
            fprintf (file, "</div><br clear='all'/>\n");
    }
    fprintf (file, "</div>");
    embed_file (file, TEMPLATE_DIR "footer.html_template");
    fclose (file);
}


void
write_html(int snippet_no)
{
    FILE *file;
    const char *name=snippet_name[snippet_no];
    char htmlfile[1024];

    sprintf (htmlfile, OUTPUT_DIR "%s.html", name);
    file = fopen (htmlfile, "w");

    embed_file (file, TEMPLATE_DIR "header.html_template");
    do_index (file, snippet_no);
            if (snippet_no>0) {
                    fprintf (file, "<div id='snippet_prev'><a href='%s.html'>prev</a></div>\n",
                                    snippet_name [snippet_no-1]);
            }
            if (snippet_no+1<snippet_count) {
                    fprintf (file, "<div id='snippet_next'><a href='%s.html'>next</a></div>\n",
                                    snippet_name [snippet_no+1]);
            }
 
    {
            const char *c=&snippet_source[snippet_no][0];

            fprintf (file, "<div id='snippet'>\n");
            /*fprintf (file, "<div id='snippet_title'><a href='snippets.html#%s'><b>%s</b></a></div>\n", name,name);*/
            fprintf (file, "<div id='snippet_image'><img src='%s.png'/></div>\n", name);
            fprintf (file, "<div id='snippet_source'><pre>");
            while (*c){
                    switch (*c){
                        case '<':
                            fprintf (file, "&lt;");
                            break;
                        case '>':
                            fprintf (file, "&gt;");
                            break;
                        case '&':
                            fprintf (file, "&amp;");
                            break;
                        default:
                            fprintf (file, "%c", *c);
                    }
                    c++;
            }
            fprintf (file, "</pre></div>\n");

            fprintf (file, "</div>\n");

   }

    embed_file (file, TEMPLATE_DIR "footer.html_template");
    fclose (file);
}



static void
embed_file (FILE *file,
            const char *filename)
{
       FILE *input_file = fopen (filename, "r");
       int read;
       char c;

       if (!input_file) {
               fprintf (stderr, "unable to read embedded file '%s'\n", filename);
               exit (-1);
       }

       while ((read=fread (&c, 1,1, input_file))){
           switch (c){
                   default:
                      fprintf (file, "%c", c);
           }
       }
       fclose (input_file);
}


