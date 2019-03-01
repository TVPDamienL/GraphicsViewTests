#include "cMainWindow.h"
#include <QtWidgets/QApplication>

#include <iostream>
#include <CL/cl.hpp>

#include "ThreadMachine.h"

static
void
OpenCLTests()
{
}



int main(int argc, char *argv[])
{
    //QApplication::setAttribute( Qt::AA_EnableHighDpiScaling ); // DPI support
    //QCoreApplication::setAttribute( Qt::AA_UseHighDpiPixmaps ); //HiDPI pixmaps
    //qputenv( "QT_SCALE_FACTOR", "2" );

    //OpenCLTests();

    cThreadProcessor::Instance();

    QApplication a(argc, argv);
    cMainWindow w;
    w.show();
    return a.exec();
}