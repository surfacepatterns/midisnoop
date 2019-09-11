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

#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <QtCore/QByteArray>
#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QVector>

#include <RtMidi.h>

class Engine: public QObject {

    Q_OBJECT

public:

    explicit
    Engine(QObject *parent=0);

    ~Engine();

    int
    getDriver() const;

    int
    getDriverCount() const;

    QString
    getDriverName(int index) const;

    bool
    getIgnoreActiveSensingEvents() const;

    bool
    getIgnoreSystemExclusiveEvents() const;

    bool
    getIgnoreTimeEvents() const;

    int
    getInputPort() const;

    int
    getInputPortCount() const;

    QString
    getInputPortName(int index) const;

    int
    getOutputPort() const;

    int
    getOutputPortCount() const;

    QString
    getOutputPortName(int index) const;

public slots:

    quint64
    sendMessage(const QByteArray &message);

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
    driverChanged(int index);

    void
    ignoreActiveSensingEventsChanged(bool ignore);

    void
    ignoreSystemExclusiveEventsChanged(bool ignore);

    void
    ignoreTimeEventsChanged(bool ignore);

    void
    inputPortAdded(int index, const QString &name);

    void
    inputPortChanged(int index);

    void
    inputPortRemoved(int index);

    void
    messageReceived(quint64 timeStamp, const QByteArray &message);

    void
    outputPortAdded(int index, const QString &name);

    void
    outputPortChanged(int index);

    void
    outputPortRemoved(int index);

private:

    static void
    handleMidiInput(double timeStamp, std::vector<unsigned char> *message,
                    void *engine);

    quint64
    getCurrentTimestamp() const;

    void
    handleMidiInput(double timeStamp,
                    const std::vector<unsigned char> &message);

    void
    removePorts();

    void
    updateEventFilter();

    int driver;
    QList<RtMidi::Api> driverAPIs;
    QStringList driverNames;
    bool ignoreActiveSensingEvents;
    bool ignoreSystemExclusiveEvents;
    bool ignoreTimeEvents;
    RtMidiIn *input;
    int inputPort;
    QStringList inputPortNames;
    RtMidiOut *output;
    int outputPort;
    QStringList outputPortNames;
    bool virtualPortsAdded;

};

#endif
