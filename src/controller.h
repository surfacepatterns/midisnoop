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

#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "aboutview.h"
#include "application.h"
#include "configureview.h"
#include "engine.h"
#include "errorview.h"
#include "mainview.h"
#include "messageview.h"

class Controller: public QObject {

    Q_OBJECT

public:

    explicit
    Controller(Application &application, QObject *parent=0);

    ~Controller();

    void
    run();

private slots:

    void
    handleDriverChange();

    void
    handleMessageSend(const QString &message);

    void
    handleReceivedMessage(quint64 timeStamp, const QByteArray &message);

private:

    QString
    getGenericDataDescription(const QByteArray &message, int lastIndex=-1);

    void
    parseMessage(const QByteArray &message);

    void
    showError(const QString &message);

    AboutView aboutView;
    Application &application;
    ConfigureView configureView;
    QString dataDescription;
    Engine engine;
    ErrorView errorView;
    MainView mainView;
    MessageView messageView;
    QString statusDescription;
    bool valid;

};

#endif
