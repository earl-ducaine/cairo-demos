#ifndef QKAPOW_H
#define QKAPOW_H

#include "qcairowidget.h"

#include <qsize.h>
#include <qstring.h>

typedef struct _cairo cairo_t;
class QRect;

class QKapow : public QCairoWidget
{
    Q_OBJECT
public:
    QKapow( QWidget *parent );

    virtual QSize sizeHint () const;

    void setText( const QString &text )
    {
        m_text = text;
    }
    int first() const
    {
        return m_first;
    }
    void setFirst( int i )
    {
        m_first = i;
    }

    cairo_t *cr() const
    {
        return m_cr;
    }
    void bend_it (double *x, double *y);

protected:
    virtual void cairoPaint( cairo_t *cr, const QRect &rect );

protected:
    void make_text_path (cairo_t *cr, double x, double y, const char *text);
    void make_star_path(cairo_t *cr);

private:
    QString   m_text;
    QSize     m_defaultSize;
    int       m_first;
    cairo_t  *m_cr;
};

#endif
