#include <QtGui/QApplication>
#include "apicovtool.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ApiCovTool w;
    w.show();
    return a.exec();
}
