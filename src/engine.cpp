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

#include <cassert>

#include <QtCore/QDateTime>
#include <QtCore/QDebug>

#include "engine.h"
#include "error.h"

// Static functions

void
Engine::handleMidiInput(double timeStamp, std::vector<unsigned char> *message,
                        void *engine)
{
    static_cast<Engine *>(engine)->handleMidiInput(timeStamp, message);
}

// Class definition

Engine::Engine(QObject *parent):
    QObject(parent)
{
    // Get available MIDI drivers.
    std::vector<RtMidi::Api> apis;
    RtMidi::getCompiledApi(apis);
    int apiCount = apis.size();

    for (int i = 0; i < apiCount; i++) {
        RtMidi::Api api = apis[i];
        switch (api) {
        case RtMidi::LINUX_ALSA:
            driverNames.append(tr("ALSA Sequencer"));
            break;
        case RtMidi::MACOSX_CORE:
            driverNames.append(tr("CoreMidi"));
            break;
        case RtMidi::UNIX_JACK:
            driverNames.append(tr("JACK Audio Connection Kit"));
            break;
        case RtMidi::WINDOWS_KS:
            driverNames.append(tr("Windows Kernel Streaming"));
            break;
        case RtMidi::WINDOWS_MM:
            driverNames.append(tr("Windows Multimedia MIDI"));
            break;
        default:
            qWarning() << tr("Unexpected MIDI API constant: %1").
                arg(static_cast<int>(api));
            // Fallthrough on purpose
        case RtMidi::RTMIDI_DUMMY:
        case RtMidi::UNSPECIFIED:
            continue;
        }
        driverAPIs.append(api);
    }

    driver = -1;
    ignoreActiveSensingEvents = true;
    ignoreSystemExclusiveEvents = true;
    ignoreTimeEvents = true;
    inputPort = -1;
    outputPort = -1;
}

Engine::~Engine()
{
    setDriver(-1);
}

int
Engine::getDriver() const
{
    return driver;
}

int
Engine::getDriverCount() const
{
    return driverAPIs.count();
}

QString
Engine::getDriverName(int index) const
{
    assert((index >= 0) && (index < driverAPIs.count()));
    return driverNames[index];
}

bool
Engine::getIgnoreActiveSensingEvents() const
{
    return ignoreActiveSensingEvents;
}

bool
Engine::getIgnoreSystemExclusiveEvents() const
{
    return ignoreSystemExclusiveEvents;
}

bool
Engine::getIgnoreTimeEvents() const
{
    return ignoreTimeEvents;
}

int
Engine::getInputPort() const
{
    return inputPort;
}

int
Engine::getInputPortCount() const
{
    return inputPortNames.count();
}

QString
Engine::getInputPortName(int index) const
{
    assert((index >= 0) && (index < inputPortNames.count()));
    return inputPortNames[index];
}

int
Engine::getOutputPort() const
{
    return outputPort;
}

int
Engine::getOutputPortCount() const
{
    return outputPortNames.count();
}

QString
Engine::getOutputPortName(int index) const
{
    assert((index >= 0) && (index < outputPortNames.count()));
    return outputPortNames[index];
}

void
Engine::handleMidiInput(double /*timeStamp*/,
                        std::vector<unsigned char> *message)
{
    quint64 timeStamp = QDateTime::currentDateTime().toMSecsSinceEpoch();
    QByteArray msg;
    int size = static_cast<int>(message->size());
    for (int i = 0; i < size; i++) {
        msg.append(message->at(i));
    }
    emit messageReceived(timeStamp, msg);
}

void
Engine::removePorts()
{
    setInputPort(-1);
    setOutputPort(-1);
    for (int i = inputPortNames.count() - 1; i >= 0; i--) {
        inputPortNames.removeAt(i);
        emit inputPortRemoved(i);
    }
    for (int i = outputPortNames.count() - 1; i >= 0; i--) {
        outputPortNames.removeAt(i);
        emit outputPortRemoved(i);
    }
}

quint64
Engine::sendMessage(const QByteArray &message)
{
    assert(outputPort != -1);
    std::vector<unsigned char> msg;
    for (int i = 0; i < message.count(); i++) {
        msg.push_back(static_cast<unsigned char>(message[i]));
    }
    try {
        output->sendMessage(&msg);
    } catch (RtError &e) {
        throw Error(e.what());
    }
    return QDateTime::currentDateTime().toMSecsSinceEpoch();
}

void
Engine::setDriver(int index)
{
    assert((index >= -1) && (index < driverAPIs.count()));
    if (driver != index) {

        // Close the currently open MIDI driver.
        if (driver != -1) {

            qDebug() << "Closing current driver ...";

            removePorts();
            delete input;
            delete output;
            driver = -1;
            emit driverChanged(-1);
        }

        // Open the new driver.
        if (index != -1) {

            qDebug() << "Opening new driver ...";

            RtMidi::Api api = driverAPIs[index];
            try {
                input = new RtMidiIn(api, "midisnoop");
                QScopedPointer<RtMidiIn> inputPtr(input);
                output = new RtMidiOut(api, "midisnoop");
                QScopedPointer<RtMidiOut> outputPtr(output);

                qDebug() << "Setting input callback ...";

                input->setCallback(handleMidiInput, this);

                // Add ports.
                try {
                    unsigned int count = input->getPortCount();
                    QString name;
                    for (unsigned int i = 0; i < count; i++) {
                        name = QString::fromStdString(input->getPortName(i));
                        inputPortNames.append(name);
                        emit inputPortAdded(i, name);
                    }
                    count = output->getPortCount();
                    for (unsigned int i = 0; i < count; i++) {
                        name = QString::fromStdString(output->getPortName(i));
                        outputPortNames.append(name);
                        emit outputPortAdded(i, name);
                    }

                    // Add a virtual port to drivers that support virtual ports.
                    switch (api) {
                    case RtMidi::LINUX_ALSA:
                    case RtMidi::MACOSX_CORE:
                    case RtMidi::UNIX_JACK:
                        name = tr("[virtual input]");
                        inputPortNames.append(name);
                        emit inputPortAdded(inputPortNames.count() - 1, name);
                        name = tr("[virtual output]");
                        outputPortNames.append(name);
                        emit outputPortAdded(outputPortNames.count() - 1, name);
                        virtualPortsAdded = true;
                        break;
                    default:
                        virtualPortsAdded = false;
                    }
                } catch (...) {
                    removePorts();
                    throw;
                }
                inputPtr.take();
                outputPtr.take();
            } catch (RtError &e) {
                throw Error(e.what());
            }
            driver = index;

            qDebug() << "Driver change complete.";

            emit driverChanged(index);
        }
    }
}

void
Engine::setIgnoreActiveSensingEvents(bool ignore)
{
    if (ignoreActiveSensingEvents != ignore) {
        ignoreActiveSensingEvents = ignore;
        updateEventFilter();
        emit ignoreActiveSensingEventsChanged(ignore);
    }
}

void
Engine::setIgnoreSystemExclusiveEvents(bool ignore)
{
    if (ignoreSystemExclusiveEvents != ignore) {
        ignoreSystemExclusiveEvents = ignore;
        updateEventFilter();
        emit ignoreSystemExclusiveEventsChanged(ignore);
    }
}

void
Engine::setIgnoreTimeEvents(bool ignore)
{
    if (ignoreTimeEvents != ignore) {
        ignoreTimeEvents = ignore;
        updateEventFilter();
        emit ignoreTimeEventsChanged(ignore);
    }
}

void
Engine::setInputPort(int index)
{
    assert((index >= -1) && (index < inputPortNames.count()));
    if (inputPort != index) {

        // Close the currently open input port.
        if (inputPort != -1) {
            try {
                input->closePort();
            } catch (RtError &e) {
                qWarning() << e.what();
            }
            inputPort = -1;
            emit inputPortChanged(-1);
        }

        // Open the new input port.
        if (index != -1) {
            try {

                qDebug() << "Opening input port " << index;

                if (virtualPortsAdded &&
                    (index == (inputPortNames.count() - 1))) {
                    input->openVirtualPort("MIDI Input");
                } else {
                    input->openPort(index, "MIDI Input");
                }
            } catch (RtError &e) {
                throw Error(e.what());
            }
            inputPort = index;
            updateEventFilter();
            emit inputPortChanged(index);
        }
    }
}

void
Engine::setOutputPort(int index)
{
    assert((index >= -1) && (index < outputPortNames.count()));
    if (outputPort != index) {

        // Close the currently open output port.
        if (outputPort != -1) {
            try {
                output->closePort();
            } catch (RtError &e) {
                qWarning() << e.what();
            }
            outputPort = -1;
            emit outputPortChanged(-1);
        }

        // Open the new output port.
        if (index != -1) {
            try {
                if (virtualPortsAdded &&
                    (index == (outputPortNames.count() - 1))) {
                    output->openVirtualPort("MIDI Output");
                } else {
                    output->openPort(index, "MIDI Output");
                }
            } catch (RtError &e) {
                throw Error(e.what());
            }
            outputPort = index;
            emit outputPortChanged(index);
        }
    }
}

void
Engine::updateEventFilter()
{
    if (inputPort != -1) {
        input->ignoreTypes(ignoreSystemExclusiveEvents, ignoreTimeEvents,
                           ignoreActiveSensingEvents);
    }
}
