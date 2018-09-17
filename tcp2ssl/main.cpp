#include "mainwindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setWindowIcon(QIcon(":/MainWindow/icon"));
    MainWindow w;
    w.show();
    return a.exec();
}
