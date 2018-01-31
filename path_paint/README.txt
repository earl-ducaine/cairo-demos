path_paint  |
------------'

A simple interactive paint core using cairo.

It displays the current stroke incrementally rendered as a path
over the composite of all previous strokes.

The concept itself could be used both for vector based drawing apps
and pixel based programs.

When painting, X can be used to switch between black and white stroke
color.

Build instructions |
-------------------'

you need gtkcairo and it's dependencies, most of this is located in the
cairo http://cairographics.org/ cvs

just running make should work fine


Not Implemented |
----------------'

Features this app would need, in approximate order of increments
to become a useful application.

    - using gtk+ 's native cairo handling
    - setting color
    - setting line width
    - setting alpha
    - load/save of png
    - load/save of svg (using stroke history)
    - changing canvas size
    - path/data simplification
    - editing sample points
    - editing of bezier points of path before use
    - closing of path
    - filling path
    - stroke/fill history (for undo)
    - zoom (redraw from stroke history)

-pippin / Øyvind Kolås   pippin@freedesktop.org
