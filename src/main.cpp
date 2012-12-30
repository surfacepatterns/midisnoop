/*
 * midisnoop - MIDI monitor and prober
 * Copyright (C) 2012 Devin Anderson
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 * Ave, Cambridge, MA 02139, USA.
 */

#include <cstdio>
#include <cstdlib>
#include <exception>

#include <QtCore/QDebug>
#include <QtCore/QLibraryInfo>
#include <QtCore/QLocale>
#include <QtCore/QTranslator>
#include <QtCore/QTextStream>

#include "controller.h"
#include "error.h"

int
main(int argc, char **argv)
{
    Application application(argc, argv);
    QString errorMessage;

    application.setApplicationName("midisnoop");
    application.setOrganizationDomain("midisnoop.googlecode.com");
    application.setOrganizationName("midisnoop.googlecode.com");

    // Translations
    QString directory = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
    QString language = QLocale::system().name();
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + language, directory);
    application.installTranslator(&qtTranslator);
    QTranslator translator;
    translator.load("midisnoop_" + language);
    application.installTranslator(&translator);
    qDebug() << application.tr("Translations loaded.");

    try {

        // Controller
        qDebug() << application.tr("Creating core application objects ...");
        Controller controller(application);
        qDebug() << application.tr("Core application objects created.");

        // Run the program
        qDebug() << application.tr("Running ...");
        controller.run();

    } catch (Error &e) {
        errorMessage = e.getMessage();
    } catch (std::exception &e) {
        errorMessage = e.what();
    }

    // Deal with errors.
    int result;
    if (errorMessage.isEmpty()) {
        qDebug() << application.tr("Exiting without errors ...");
        result = EXIT_SUCCESS;
    } else {
        QTextStream(stderr) << application.tr("Error: %1\n").arg(errorMessage);
        result = EXIT_FAILURE;
    }

    // Cleanup
    qDebug() << application.tr("Unloading translations ...");
    application.removeTranslator(&translator);
    application.removeTranslator(&qtTranslator);

    return result;
}
