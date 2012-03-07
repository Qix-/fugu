/**
 * \file
 * \brief The entry point to fugu
 * \author ben
  *
 * \cond showlicense
 * \verbatim
 * --------------------------------------------------------------
 *    ___
 *   |  _|___
 *   |  _| . | fg: real-time procedural
 *   |_| |_  | animation and generation
 *       |___| of 3D forms
 *
 *   Copyright (c) 2012 Centre for Electronic Media Art (CEMA)
 *   Monash University, Australia. All rights reserved.
 *
 *   Use of this software is governed by the terms outlined in
 *   the LICENSE file.
 *
 * --------------------------------------------------------------
 * \endverbatim
 * \endcond
 */

#include <QApplication>
#include <QCleanlooksStyle>
#include "mainwindow.h"
#include <QtOpenGL>
#include <QSettings>


 int main(int argc, char *argv[])
 {
     QApplication app(argc, argv);
     QApplication::setStyle(new QCleanlooksStyle);

     QGLFormat glFormat(QGL::DepthBuffer);
     glFormat.setSampleBuffers(true);

     /*
     glFormat.setVersion( 3, 3 );
     glFormat.setProfile( QGLFormat::CompatibilityProfile );
     */
     QGLFormat::setDefaultFormat(glFormat);

     QSettings settings("MonashUniversity", "Fugu");

     MainWindow window;

     bool maximised = settings.value("window/maximised", false).toBool();
     if (maximised)
    	 window.showMaximized();
     else
    	 window.show();
     return app.exec();
 }
