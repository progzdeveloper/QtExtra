#include "progresswidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ProgressWidget w;
    w.show();

    return a.exec();
}
