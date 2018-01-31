#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>

class QKapow;
class ControlWidgetBase;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();

private slots:
    void updateWidget();
private:
    void createGUI();
    void createActions();

private:
    QKapow *m_qcairo;
    ControlWidgetBase *m_control;
};

#endif
