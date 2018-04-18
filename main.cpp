#include "mainwindow.h"
#include "releasemainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#ifdef QT_NO_DEBUG
    ReleaseMainWindow w;
    w.show();
#else
    MainWindow w;
    w.show();
#endif

    return a.exec();
}
