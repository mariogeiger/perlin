#include <QtGui/QApplication>
#include "glwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qsrand(a.applicationPid());

    GLWidget w;
    w.show();
    
    return a.exec();
}
