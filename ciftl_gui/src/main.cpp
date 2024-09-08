#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    std::srand((unsigned int)std::time(NULL));
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
