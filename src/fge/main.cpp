#include <QApplication>
#include "mainwindow.h"

 int main(int argc, char *argv[])
 {
     QApplication app(argc, argv);
     MainWindow window;
     window.resize(640, 512);
     window.show();
     return app.exec();
 }
