#include "qcairowidget.h"

QCairoWidget::QCairoWidget( QWidget *parent, const char *name )
    : QWidget( parent, name )
{
    setDoubleBuffered( true );
    m_surf = cairo_xlib_surface_create ((Display *) x11AppDisplay(), (Drawable) handle(), (Visual *) x11Visual(), width(), height() );
    m_cr = cairo_create(m_surf);
    cairo_surface_destroy(m_surf);
}

QCairoWidget::~QCairoWidget()
{
    cairo_destroy( m_cr );
    m_cr = 0;
}

void QCairoWidget::setDoubleBuffered( bool db )
{
    m_doubleBuffer = db;
    if ( m_doubleBuffer )
        setBackgroundMode( Qt::NoBackground );
}

void QCairoWidget::cairoPaint( cairo_t *, const QRect & )
{

}

void QCairoWidget::paintEvent( QPaintEvent *e )
{
    //the save/restore is a little silly...
    cairo_save( m_cr );

    Display *dpy = x11AppDisplay();
    Drawable drw = handle();

    if ( m_doubleBuffer ) {
        m_buffer = QPixmap( size() );
        m_buffer.fill( this, 0, 0 );
        dpy = m_buffer.x11AppDisplay();
        drw = m_buffer.handle();
    }

    m_surf = cairo_get_target (m_cr);
    cairo_xlib_surface_set_drawable (m_surf, drw, width(), height());

    cairoPaint( m_cr, e->rect() );

    if ( cairo_status( m_cr ) != CAIRO_STATUS_SUCCESS ) {
        qDebug("Cairo is unhappy: %s\n", cairo_status_to_string( cairo_status( m_cr ) ) );
    }

    if ( m_doubleBuffer )
        bitBlt( this, 0, 0, &m_buffer );

    cairo_restore( m_cr );
}
