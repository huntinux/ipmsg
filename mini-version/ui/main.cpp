#include "maindialog.h"
#include <QApplication>
#include "classicmaindialog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainDialog w;
    //w.show();
    ClassicMainDialog w;
    w.show();

    return a.exec();
}
