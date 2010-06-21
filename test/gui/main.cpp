/** ===========================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * @date  : 2010-06-21
 * @note  : GUI test program for libkface
 *
 * @author: Copyright (C) 2010 by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 *          Copyright (C) 2010 by Alex Jironkin <alexjironkin at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// Qt includes

#include <QtGui/QApplication>

// Local includes

#include "mainwindow.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
