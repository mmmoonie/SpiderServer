#include "mainwindow.h"
#include "releasemainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();
    ReleaseMainWindow w;
    w.show();

    return a.exec();
}
