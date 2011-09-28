#include <QApplication>
#include <QCleanlooksStyle>
#include "mainwindow.h"

 int main(int argc, char *argv[])
 {
     QApplication app(argc, argv);
     QApplication::setStyle(new QCleanlooksStyle);
     MainWindow window;
     window.resize(640, 512);
     window.show();
     return app.exec();
 }
