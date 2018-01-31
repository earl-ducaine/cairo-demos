#include "cairo_custom.h"

void
cairo_rectangle_round (cairo_t *cr,
                       double x0,    double y0,
                       double width, double height,
                       double radius)
{
  double x1,y1;

  x1=x0+width;
  y1=y0+height;

  if (!width || !height)
    return;
  if (width/2<radius)
    {
      if (height/2<radius)
        {
          cairo_move_to  (cr, x0, (y0 + y1)/2);
          cairo_curve_to (cr, x0 ,y0, x0, y0, (x0 + x1)/2, y0);
          cairo_curve_to (cr, x1, y0, x1, y0, x1, (y0 + y1)/2);
          cairo_curve_to (cr, x1, y1, x1, y1, (x1 + x0)/2, y1);
          cairo_curve_to (cr, x0, y1, x0, y1, x0, (y0 + y1)/2);
        }
      else
        {
          cairo_move_to  (cr, x0, y0 + radius);
          cairo_curve_to (cr, x0 ,y0, x0, y0, (x0 + x1)/2, y0);
          cairo_curve_to (cr, x1, y0, x1, y0, x1, y0 + radius);
          cairo_line_to (cr, x1 , y1 - radius);
          cairo_curve_to (cr, x1, y1, x1, y1, (x1 + x0)/2, y1);
          cairo_curve_to (cr, x0, y1, x0, y1, x0, y1- radius);
        }
    }
  else
    {
      if (height/2<radius)
        {
          cairo_move_to  (cr, x0, (y0 + y1)/2);
          cairo_curve_to (cr, x0 , y0, x0 , y0, x0 + radius, y0);
          cairo_line_to (cr, x1 - radius, y0);
          cairo_curve_to (cr, x1, y0, x1, y0, x1, (y0 + y1)/2);
          cairo_curve_to (cr, x1, y1, x1, y1, x1 - radius, y1);
          cairo_line_to (cr, x0 + radius, y1);
          cairo_curve_to (cr, x0, y1, x0, y1, x0, (y0 + y1)/2);
        }
      else
        {
          cairo_move_to  (cr, x0, y0 + radius);
          cairo_curve_to (cr, x0 , y0, x0 , y0, x0 + radius, y0);
          cairo_line_to (cr, x1 - radius, y0);
          cairo_curve_to (cr, x1, y0, x1, y0, x1, y0 + radius);
          cairo_line_to (cr, x1 , y1 - radius);
          cairo_curve_to (cr, x1, y1, x1, y1, x1 - radius, y1);
          cairo_line_to (cr, x0 + radius, y1);
          cairo_curve_to (cr, x0, y1, x0, y1, x0, y1- radius);
        }
    }

  cairo_close_path (cr);
}

void
cairo_edgeline (cairo_t *cr,
                double x0,
                double y0,
                double x1,
                double y1,
                double backoff)
{
  y0 = y0 - 1;
  y1 = y1 + 1;

  cairo_move_to (cr, x0, y0);
  if (y1 > y0 + backoff / 2)
    {
      cairo_curve_to (cr, x0, (y0 + y1) / 2, x1, (y0 + y1) / 2, x1, y1);
    }
  else
    {
      cairo_curve_to (cr, x0,
                      y0 + (backoff / 4 + (y0 + backoff / 2 - y1)), x1,
                      y1 - (backoff / 4 + (y0 + backoff / 2 - y1)), x1,
                      y1);
    }
}
