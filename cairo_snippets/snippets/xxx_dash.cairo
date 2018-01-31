/* BUG: spline is not stroked, is it a bug that
   a negative offset for cairo_set_dash, causes
   no dashing to occur?
*/

double dashes[] = {0.20,  /* ink */
                   0.05,  /* skip */
                   0.05,  /* ink */
                   0.05   /* skip*/ 
                  };
int    ndash  = sizeof (dashes)/sizeof(dashes[0]); 
double offset = -0.2; 

snippet_normalize (cr, width, height);

cairo_set_dash (cr, dashes, ndash, offset);

cairo_move_to (cr, 0.5, 0.1);
cairo_line_to (cr, 0.9, 0.9);
cairo_rel_line_to (cr, -0.4, 0.0);
cairo_curve_to (cr, 0.2, 0.9, 0.2, 0.5, 0.5, 0.5);

cairo_stroke (cr);
