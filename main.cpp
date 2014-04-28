#include <QtGui/QApplication>
#include "Testy jezykowe.h"


int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    Testy_jezykowe testy jezykowe;
    testy jezykowe.show();
    return app.exec();
}
