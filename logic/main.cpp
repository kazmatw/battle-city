#include "qmlmain.h"

#include <QGuiApplication>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    Tanks::QMLMain q;

    return app.exec();
}
