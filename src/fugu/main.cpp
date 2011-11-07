#include <QApplication>
#include <QCleanlooksStyle>
#include "mainwindow.h"
#include <QtOpenGL>

 int main(int argc, char *argv[])
 {
     QApplication app(argc, argv);
     QApplication::setStyle(new QCleanlooksStyle);

     QGLFormat glFormat(QGL::DepthBuffer);
     /*
     glFormat.setVersion( 3, 3 );
     glFormat.setProfile( QGLFormat::CompatibilityProfile );
     */
     QGLFormat::setDefaultFormat(glFormat);

     MainWindow window;
     window.resize(640, 512);
     window.show();
     return app.exec();
 }
