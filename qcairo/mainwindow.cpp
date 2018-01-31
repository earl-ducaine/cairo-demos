#include "mainwindow.h"

#include "controlwidgetbase.h"
#include "qkapow.h"

#include <qapplication.h>
#include <qscrollview.h>
#include <qspinbox.h>
#include <qdockwindow.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qaction.h>

MainWindow::MainWindow()
    : QMainWindow( 0 )
{
    createGUI();
    createActions();
}

void MainWindow::createGUI()
{
    QScrollView *view = new QScrollView( this );
    m_qcairo = new QKapow( view );
    view->addChild( m_qcairo );
    setCentralWidget( view );

    QDockWindow *dockWindow = new QDockWindow( QDockWindow::InDock, this,
                                               "controldock" );
    dockWindow->setResizeEnabled( true );
    m_control = new ControlWidgetBase( dockWindow, "controlwidget" );
    dockWindow->setWidget( m_control );
    addDockWindow( dockWindow, Qt::DockLeft );

    m_control->m_width->setMinValue( 100 );
    m_control->m_height->setMinValue( 100 );
    m_control->m_width->setMaxValue( 1000 );
    m_control->m_height->setMaxValue( 1000 );

    m_control->m_textEdit->setText( "sample" );
    m_control->m_width->setValue( m_qcairo->width() );
    m_control->m_height->setValue( m_qcairo->height() );
    m_control->m_dbCB->setChecked( m_qcairo->doubleBuffered() );

    connect( m_control->m_updateBtn, SIGNAL(clicked()),
             SLOT(updateWidget()) );
}

void MainWindow::createActions()
{
    QPopupMenu *file = new QPopupMenu( this );
    menuBar()->insertItem( "&File", file );

    file->insertItem( "&Quit", qApp, SLOT(quit()) );
}

void MainWindow::updateWidget()
{
    m_qcairo->setText( m_control->m_textEdit->text() );
    m_qcairo->setDoubleBuffered( m_control->m_dbCB->isChecked() );
    m_qcairo->resize( m_control->m_width->value(),
                      m_control->m_height->value() );
    m_qcairo->update();
}
