/*
 * File:    main.cpp
 * Author:  Rachel Bood 100088769
 * Date:    2014/11/07
 * Version: 1.2
 *
 * Purpose: executes the mainwindow.ui file
 *
 * Modification history:
 * May 8, 2020 (IC/JD V1.1)
 * - Add called to addApplicationFont, since newer versions of Qt (apparently)
 *   need this to use the included fonts.
 * May 12, 2020 (IC V1.2)
 *  (a) Added another font (arimo.ttf) to be embedded, so that Graphic
 *      can look as similar as possible on different systems. 
 */

#include "mainwindow.h"
#include <QApplication>
#include <QFileSystemModel>
#include <QTreeView>
#include <QFontDatabase>


int
main(int argc, char * argv[])
{
    QApplication a(argc, argv);

    QFontDatabase::addApplicationFont(":/fonts/cmmi10.ttf");
    QFontDatabase::addApplicationFont(":/fonts/cmr10.ttf");
    QFontDatabase::addApplicationFont(":/fonts/arimo.ttf");

    MainWindow w;
    w.show();
    return a.exec();
}
