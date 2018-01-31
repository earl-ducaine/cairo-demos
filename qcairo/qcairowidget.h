#ifndef QCAIROWIDGET_H
#define QCAIROWIDGET_H

#include <qwidget.h>
#include <qpixmap.h>

#include <cairo.h>
#include <cairo-xlib.h>

class QCairoWidget : public QWidget
{
    Q_OBJECT
public:
    QCairoWidget( QWidget *parent, const char *name=0 );
    ~QCairoWidget();

    bool doubleBuffered() const;
    void setDoubleBuffered( bool db );

protected:
    virtual void cairoPaint( cairo_t *cr, const QRect &invalidatedArea );

protected:
    void paintEvent( QPaintEvent *e );

private:
    cairo_t *m_cr;
    cairo_surface_t * m_surf;
    QPixmap m_buffer;
    bool m_doubleBuffer;
};

inline bool QCairoWidget::doubleBuffered() const
{
    return m_doubleBuffer;
}

#endif
