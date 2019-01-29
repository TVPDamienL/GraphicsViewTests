#include "cMainWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    //QApplication::setAttribute( Qt::AA_EnableHighDpiScaling ); // DPI support
    //QCoreApplication::setAttribute( Qt::AA_UseHighDpiPixmaps ); //HiDPI pixmaps
    //qputenv( "QT_SCALE_FACTOR", "2" );


    QApplication a(argc, argv);
    cMainWindow w;
    w.show();
    return a.exec();
}

