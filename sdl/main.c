/* 
   sdl - SDL Demo for Cairographics

   Copyright (C) 2004 Eric Windisch

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  

#include <termios.h>
#include <grp.h>
#include <pwd.h>
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include <SDL.h>
#include <cairo.h>
#include <svg.h>
#include <svg-cairo.h>

/* load_svg: This is what you're probably interested in!
 * -----------------------------------------------------
 *  If width and height are greater than 0, the image will
 *  be scaled to that size. wscale and hscale would be ignored.
 *
 *  If width and height are less than 1, wscale and hscale will
 *  resize the width and the height to the specified scales.
 *
 *  If width and height are less than 1, and wscale and hscale
 *  are either 0 or 1, then the image will be loaded at it's
 *  natural size.
 *
 *  See main() for examples.
 */
SDL_Surface *load_svg (char *file, int width, int height, float wscale, float hscale) {
    svg_cairo_t *scr;
    int bpp;
    int btpp;
    unsigned int rwidth;
    unsigned int rheight;

    // Create the SVG cairo stuff. 
    svg_cairo_create(&scr);
    svg_cairo_parse (scr, file);

    if (wscale <= 0) {
        wscale=1;
    }
    if (hscale <= 0) {
        hscale=1;
    }

    /*Now get the width and height of the SVG content, so we can calculate and adjust our SDL surface dimensions */
    svg_cairo_get_size (scr, &rwidth, &rheight);

    /*Calculate final width and height of our surface based on the parameters passed */
    if (width > 0) {
        if (rwidth>width) {
            wscale=(float)width/(float)rwidth;
            printf ("rwidth/width = %f\n", wscale);
        } else {
            wscale=(float)rwidth/(float)width;
            printf ("width/wwidth = %f\n", wscale);
        }
    } else {
        width=(int)(rwidth*wscale);
        printf ("width = %i\n", width);
    }
    if (height > 0) {
        if (rheight>height) {
            hscale=(float)height/(float)rheight;
            printf ("height/rheight = %f\n", hscale);
        } else {
            hscale=(float)rheight/(float)height;
            printf ("rheight/height = %f\n", hscale);
        }
    } else {
        height=(int)(rheight*hscale);
        printf ("height = %i\n", height);
    }
    
    /* We will create a CAIRO_FORMAT_ARGB32 surface. We don't need to match
    the screen SDL format, but we are interested in the alpha bit */
    bpp=32; /*bits per pixel*/
    btpp=4; /*bytes per pixel*/

    /* scanline width */ 
    int stride=width * btpp;

    /* Allocate an image */
    unsigned char *image=calloc(stride*height, 1);

    /* Create the cairo surface with the adjusted width and height */
    cairo_surface_t *cairo_surface;
    cairo_surface = cairo_image_surface_create_for_data (image,
							 CAIRO_FORMAT_ARGB32,
							 width, height, stride);

    cairo_t *cr=cairo_create(cairo_surface);
    cairo_scale (cr, wscale, hscale);

    /* Render SVG to our surface */
    svg_cairo_render (scr, cr);

    /* Cleanup cairo */
    cairo_surface_destroy (cairo_surface);
    cairo_destroy (cr);

    /*Destroy the svg_cairo structure */    
    svg_cairo_destroy (scr);

    /*Adjust the SDL surface mask to ARGB, matching the cairo surface created.*/
    Uint32 rmask, gmask, bmask, amask;

    rmask = 0x00ff0000;
    gmask = 0x0000ff00;
    bmask = 0x000000ff;
    amask = 0xff000000;
    
    /* Create the SDL surface using the pixel data stored. It will automatically be set to use alpha using these mask values */
    SDL_Surface *sdl_surface=SDL_CreateRGBSurfaceFrom( (void *) image, width, height, bpp, stride, rmask, gmask, bmask, amask);

    /* This is not important in this demo, but in a more complex application we should probably reference and free the SDL surface when we are done with it to avoid memory leaks. */
    return sdl_surface;
}

SDL_Surface*
init_SDL (int width, int height, int bpp)
{
    SDL_Surface *screen;
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit);
    
    /* Finished initalizing SDL */
    screen = SDL_SetVideoMode(width, height, bpp, SDL_SWSURFACE);
    if ( screen == NULL ) {
        fprintf(stderr, "Unable to set %ix%i video: %s\n", width, height, SDL_GetError());
        exit(1);
    }

    /* Give a window title. */
    SDL_WM_SetCaption("Cairo SDL DEMO", "DEMO");
    return screen;
}
int
main (int argc, char **argv)
{
    SDL_Surface *screen;
    SDL_Surface *svgimg;
    SDL_Event event;

    int width=320;
    int height=240;

    screen=init_SDL(width,height,16);
    /*Fill the background in purple so we can see the alpha blend */
    SDL_FillRect (screen, NULL, SDL_MapRGB(screen->format,255,0,255));

    /* Width and height are non-zero, will scale image to width x height */
    svgimg=load_svg ("demo.svg", width, height, 0, 0);

    /* Will scale image by 2x2.
    //svgimg=load_svg ("demo.svg", 0, 0, 2, 2);

    // Will load image at it's natural size
    //svgimg=load_svg ("demo.svg", 0, 0, 0, 0);
    //svgimg=load_svg ("demo.svg", 0, 0, 1, 1); */


    SDL_BlitSurface (svgimg, NULL, screen, NULL);
    SDL_UpdateRect (screen, 0, 0, 0, 0);

    /* Handle events so we can safely exit. */
    while (1) {
            while (SDL_PollEvent (&event)) {
                switch (event.type) {
                    case SDL_KEYDOWN:
                        if (event.key.keysym.sym == SDLK_ESCAPE)
                            exit (0);
                        else if (event.key.keysym.sym == SDLK_f)
                            /* press f to toggle fullscreen */
                            SDL_WM_ToggleFullScreen(screen);
                        break;
                    case SDL_QUIT:
                        exit (0);
                        break;
                }
            }
    }
    exit (0);
}
