#include "mainwindow.h"

#include <qapplication.h>

int main( int argc, char **argv )
{
    QApplication app( argc, argv );

    MainWindow *w = new MainWindow();

    app.setMainWidget( w );
    w->setMinimumSize( 200, 200 );
    w->show();
    return app.exec();
}
