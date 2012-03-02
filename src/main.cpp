/**************************************************************************************
 *                                   LJ compiler                                      *
 *                                                                                    *
 * (c) copyright 2011, lmh                                                            *
 * All rights reserved                                                                *
 *                                                                                    *
 * File   : main.cpp                                                                  *
 * Details: Main program for LJ compiler                                              *
 *                                                                                    *
 * Name   : lmh                                                                       *
 * Date   : 2011.5.21                                                                 *
 * Version: 0.0                                                                       *
 **************************************************************************************/

/* interface include */
#include "mainwindow.h"
#include <QtGui>

int
main( int argc, char *argv[] )
{
    QApplication app( argc, argv );

    QApplication::setStyle( QStyleFactory::create( "plastique" ) );
    QPixmap pixmap( ":/images/splash.png" );
    QSplashScreen splash( pixmap );

    splash.show();
    MainWindow window;
    window.showMaximized();
    splash.finish( &window );

    return app.exec();
}
