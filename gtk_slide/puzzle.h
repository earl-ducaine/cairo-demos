#ifndef PUZZLE_H
#define PUZZLE_H

#include <gtk/gtk.h>

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#define PUZZLE(obj)          GTK_CHECK_CAST (obj, puzzle_get_type (), Puzzle)
#define PUZZLE_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, puzzle_get_type (), PuzzleClass)
#define IS_PUZZLE(obj)       GTK_CHECK_TYPE (obj, puzzle_get_type ())

    typedef struct _Puzzle       Puzzle;
    typedef struct _PuzzleClass  PuzzleClass;

    GtkType puzzle_get_type (void);

    GtkWidget *puzzle_new (void);

#ifdef __cplusplus

}
#endif                          /* __cplusplus */
#endif                          /* PUZZLE_H */
