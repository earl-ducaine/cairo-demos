/*
 * Copyright © 2005 Calum Robinson
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Calum Robinson not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Calum Robinson makes no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * CALUM ROBINSON DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL CALUM ROBINSON BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include <Carbon/Carbon.h>

#include <cairo.h>
#include <cairo-quartz.h>

#include "fdface.h"
#include "fdhand.h"
#include "fdlogo.h"

Rect	    gRect;
int	    width = 400;
int	    height = 400;
WindowRef   gTestWindow;

pascal void RedrawTimerCallback(EventLoopTimerRef theTimer, void* userData);

OSStatus CreateTestWindow(void);

static pascal OSStatus TestWindowEventHandler( EventHandlerCallRef inCallRef, EventRef inEvent, void* inUserData )
{
    OSStatus	err		= eventNotHandledErr;
    UInt32	eventKind       = GetEventKind(inEvent);
    UInt32	eventClass      = GetEventClass(inEvent);
    WindowRef	window		= (WindowRef)inUserData;
    
    
    switch (eventClass)
    {
	case kEventClassWindow:
	    if (eventKind == kEventWindowDrawContent)
	    {
		CGContextRef	context;
		cairo_t		*cr;
		cairo_surface_t *surface;
		int		width, height;


		width = gRect.right - gRect.left;
		height = gRect.bottom - gRect.top;

		QDBeginCGContext(GetWindowPort(window), &context);

		surface = cairo_quartz_surface_create(context, width, height);
		cr = cairo_create(surface);


	    #if 1
		
		// I'm not sure if we could call CGContextClearRect in Cairo itself in
		// any useful way - it's only really useful for overlay windows...
		CGContextClearRect(context, CGRectMake(0,0, width, height));
		
		
		// Draw the freedesktop.org clock
		//
		// TODO: cache the face and just draw the hands each time
		fdface_draw(cr, width, height);

		fdhand_draw_now(cr, width, height, true);

	    #else
		    
		// Draw one of the Cairo snippets
		cairo_scale(cr, width/1.0, height/1.0);
		cairo_rectangle(cr, 0,0, 1.0, 1.0);
		cairo_set_source_rgb(cr, 1,1,1);
		cairo_fill(cr);
		cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
		cairo_set_line_width(cr, 0.04);
		
		#if 1
		    cairo_text_extents_t extents;

		    const char *utf8 = "cairo";
		    double x,y;

		    cairo_select_font_face(cr, "Times",
					   CAIRO_FONT_SLANT_NORMAL,
					   CAIRO_FONT_WEIGHT_NORMAL);

		    cairo_set_font_size(cr, 0.4);
		    cairo_text_extents(cr, utf8, &extents);

		    x=0.1;
		    y=0.6;

		    cairo_move_to(cr, x,y);
		    cairo_show_text(cr, utf8);

		    // draw helping lines
		    cairo_set_source_rgba(cr, 1,0.2,0.2,0.6);
		    cairo_arc(cr, x, y, 0.05, 0, 2*M_PI);
		    cairo_fill(cr);
		    cairo_move_to(cr, x,y);
		    cairo_rel_line_to(cr, 0, -extents.height);
		    cairo_rel_line_to(cr, extents.width, 0);
		    cairo_rel_line_to(cr, extents.x_bearing, -extents.y_bearing);
		    cairo_stroke(cr);
		#elif 0
		    cairo_select_font(cr, "Courier", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
		    cairo_scale_font(cr, 0.35);

		    cairo_move_to(cr, 0.04, 0.53);
		    cairo_show_text(cr, "Hello");

		    cairo_move_to(cr, 0.27, 0.65);
		    cairo_text_path(cr, "void");
		    cairo_save(cr);
		    cairo_set_rgb_color(cr, 0.5,0.5,1);
		    cairo_fill(cr);
		    cairo_restore(cr);
		    cairo_set_line_width(cr, 0.01);
		    cairo_stroke(cr);

		    /* draw helping lines */
		    cairo_set_rgb_color(cr, 1,0.2,0.2);
		    cairo_set_alpha(cr, 0.6);
		    cairo_arc(cr, 0.04, 0.53, 0.02, 0, 2*M_PI);
		    cairo_arc(cr, 0.27, 0.65, 0.02, 0, 2*M_PI);
		    cairo_fill(cr);
		#elif 1
		    cairo_arc(cr, 0.5, 0.5, 0.5, 0, 2 * M_PI);
		    cairo_clip(cr);

		    cairo_new_path(cr);  /* current path is not consumed by cairo_clip() */
		    cairo_rectangle(cr, 0, 0, 1, 1);
		    cairo_fill(cr);
		    cairo_set_rgb_color(cr, 0, 1, 0);
		    cairo_move_to(cr, 0, 0);
		    cairo_line_to(cr, 1, 1);
		    cairo_move_to(cr, 1, 0);
		    cairo_line_to(cr, 0, 1);
		    cairo_stroke(cr);
		#endif
	    #endif


		cairo_destroy(cr);
		
		CGContextFlush(context);

		QDEndCGContext(GetWindowPort(window), &context);
		
		
		err = noErr;
	    }
	    else if (eventKind == kEventWindowClickContentRgn)
	    {
		Point	mouseLoc;
		
		GetGlobalMouse(&mouseLoc);
		DragWindow(window, mouseLoc, NULL);
	    }
	    break;
    }

    return err;
}


pascal void RedrawTimerCallback(EventLoopTimerRef theTimer, void* userData)
{
    Rect    windowRect;
    
    
    SetRect(&windowRect, 0, 0, width, height);
    
    InvalWindowRect(gTestWindow, &windowRect);
}


OSStatus CreateTestWindow(void)
{
    const EventTypeSpec	windowEvents[]	=   {   { kEventClassWindow,    kEventWindowClickContentRgn },
						{ kEventClassWindow, 	kEventWindowDrawContent }
    };

    static EventHandlerUPP		gTestWindowEventProc = NULL;
    OSStatus				err;
    EventLoopTimerRef			redrawTimer;


    SetRect(&gRect, 30, 60, 30 + width, 60 + height);

    err = CreateNewWindow(  kOverlayWindowClass,
			    kWindowStandardHandlerAttribute | kWindowAsyncDragAttribute,
			    &gRect,
			    &gTestWindow);

    if (err == noErr)
    {
	if (gTestWindowEventProc == NULL)
	{
	    gTestWindowEventProc = NewEventHandlerUPP(TestWindowEventHandler);
	    if (gTestWindowEventProc == NULL)
		return memFullErr;		// We are screwed. Really.
	}

	err = InstallWindowEventHandler(    gTestWindow,
					    gTestWindowEventProc,
					    GetEventTypeCount(windowEvents),
					    windowEvents,
					    gTestWindow,
					    NULL);

	ShowWindow(gTestWindow);
    }
    
    
    err = InstallEventLoopTimer(    GetMainEventLoop(),
				    0.0,
				    1 * kEventDurationSecond,
				    NewEventLoopTimerUPP(RedrawTimerCallback),
				    NULL,
				    &redrawTimer);
    

    return err;
}


int main(int argc, char* argv[])
{
    OSStatus	err;

    err = CreateTestWindow();

    // Call the event loop
    RunApplicationEventLoop();

    return err;
}

