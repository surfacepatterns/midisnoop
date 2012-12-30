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

#ifndef __CONFIGUREVIEW_H__
#define __CONFIGUREVIEW_H__

#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QPushButton>

#include "designerview.h"

class ConfigureView: public DesignerView {

    Q_OBJECT

public:

    explicit
    ConfigureView(QObject *parent=0);

    ~ConfigureView();

public slots:

    void
    addDriver(int index, const QString &name);

    void
    addInputPort(int index, const QString &name);

    void
    addOutputPort(int index, const QString &name);

    void
    removeInputPort(int index);

    void
    removeOutputPort(int index);

    void
    setDriver(int index);

    void
    setIgnoreActiveSensingEvents(bool ignore);

    void
    setIgnoreSystemExclusiveEvents(bool ignore);

    void
    setIgnoreTimeEvents(bool ignore);

    void
    setInputPort(int index);

    void
    setOutputPort(int index);

signals:

    void
    driverChangeRequest(int index);

    void
    ignoreActiveSensingEventsChangeRequest(bool ignore);

    void
    ignoreSystemExclusiveEventsChangeRequest(bool ignore);

    void
    ignoreTimeEventsChangeRequest(bool ignore);

    void
    inputPortChangeRequest(int index);

    void
    outputPortChangeRequest(int index);

private slots:

    void
    handleDriverActivation(int index);

    void
    handleInputPortActivation(int index);

    void
    handleOutputPortActivation(int index);

private:

    QPushButton *closeButton;
    QComboBox *driver;
    QCheckBox *ignoreActiveSensingEvents;
    QCheckBox *ignoreSystemExclusiveEvents;
    QCheckBox *ignoreTimeEvents;
    QComboBox *inputPort;
    QComboBox *outputPort;

};

#endif
