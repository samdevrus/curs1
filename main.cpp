#include "mainwindow.h"
#include "DarkStyle.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle(new DarkStyle());

    MainWindow w;
    w.show();

    return a.exec();
}
