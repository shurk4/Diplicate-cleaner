#include "mainwindow.h"
#include "mainmaster.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

//    MainMaster z;
//    z.show();
    return a.exec();
}
