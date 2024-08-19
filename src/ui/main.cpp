#include "main.h"
#include <QApplication>

MainWindow * w;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // MainWindow w(argc, argv);
    w = new MainWindow(argc, argv);
    w->show();
    return a.exec();
}
