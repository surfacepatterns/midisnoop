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

#include <QtCore/QDebug>

#include "controller.h"
#include "error.h"
#include "util.h"

// Static data

const int statusLengths[0x80] = {
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    0, 2, 3, 2, -1, -1, 1, -1, 1, 1, 1, 1, 1, -1, 1, 1
};

// Class definition

Controller::Controller(Application &application, QObject *parent):
    QObject(parent),
    application(application)
{
    // Setup about view
    aboutView.setMajorVersion(MIDISNOOP_MAJOR_VERSION);
    aboutView.setMinorVersion(MIDISNOOP_MINOR_VERSION);
    aboutView.setRevision(MIDISNOOP_REVISION);
    connect(&aboutView, SIGNAL(closeRequest()),
            &aboutView, SLOT(hide()));

    // Setup configure view
    int driverCount = engine.getDriverCount();
    if (! driverCount) {
        throw Error(tr("no MIDI drivers found"));
    }
    for (int i = 0; i < driverCount; i++) {
        configureView.addDriver(i, engine.getDriverName(i));
    }
    int driver = engine.getDriver();
    int outputPort = engine.getOutputPort();
    configureView.setDriver(driver);
    configureView.setInputPort(engine.getInputPort());
    configureView.setIgnoreActiveSensingEvents
        (engine.getIgnoreActiveSensingEvents());
    configureView.setIgnoreSystemExclusiveEvents
        (engine.getIgnoreSystemExclusiveEvents());
    configureView.setIgnoreTimeEvents(engine.getIgnoreTimeEvents());
    configureView.setOutputPort(outputPort);
    connect(&configureView, SIGNAL(closeRequest()),
            &configureView, SLOT(hide()));
    connect(&configureView, SIGNAL(driverChangeRequest(int)),
            &engine, SLOT(setDriver(int)));
    connect(&configureView,
            SIGNAL(ignoreActiveSensingEventsChangeRequest(bool)),
            &engine, SLOT(setIgnoreActiveSensingEvents(bool)));
    connect(&configureView,
            SIGNAL(ignoreSystemExclusiveEventsChangeRequest(bool)),
            &engine, SLOT(setIgnoreSystemExclusiveEvents(bool)));
    connect(&configureView, SIGNAL(ignoreTimeEventsChangeRequest(bool)),
            &engine, SLOT(setIgnoreTimeEvents(bool)));
    connect(&configureView, SIGNAL(inputPortChangeRequest(int)),
            &engine, SLOT(setInputPort(int)));
    connect(&configureView, SIGNAL(outputPortChangeRequest(int)),
            &engine, SLOT(setOutputPort(int)));

    // Setup error view
    connect(&errorView, SIGNAL(closeRequest()),
            &errorView, SLOT(hide()));

    // Setup main view
    mainView.setMessageSendEnabled((driver != -1) && (outputPort != -1));
    connect(&mainView, SIGNAL(aboutRequest()),
            &aboutView, SLOT(show()));
    connect(&mainView, SIGNAL(addMessageRequest()),
            &messageView, SLOT(show()));
    connect(&mainView, SIGNAL(clearMessagesRequest()),
            &mainView, SLOT(clearMessages()));
    connect(&mainView, SIGNAL(configureRequest()),
            &configureView, SLOT(show()));
    connect(&mainView, SIGNAL(closeRequest()),
            &application, SLOT(quit()));

    // Setup message view
    connect(&messageView, SIGNAL(closeRequest()),
            &messageView, SLOT(hide()));
    connect(&messageView, SIGNAL(sendRequest(const QString &)),
            &messageView, SLOT(hide()));
    connect(&messageView, SIGNAL(sendRequest(const QString &)),
            SLOT(handleMessageSend(const QString &)));

    // Setup engine
    connect(&engine, SIGNAL(messageReceived(quint64, const QByteArray &)),
            SLOT(handleReceivedMessage(quint64, const QByteArray &)));
    connect(&engine, SIGNAL(driverChanged(int)),
            &configureView, SLOT(setDriver(int)));
    connect(&engine, SIGNAL(driverChanged(int)),
            SLOT(handleDriverChange()));
    connect(&engine, SIGNAL(ignoreActiveSensingEventsChanged(bool)),
            &configureView, SLOT(setIgnoreActiveSensingEvents(bool)));
    connect(&engine, SIGNAL(ignoreSystemExclusiveEventsChanged(bool)),
            &configureView, SLOT(setIgnoreSystemExclusiveEvents(bool)));
    connect(&engine, SIGNAL(ignoreTimeEventsChanged(bool)),
            &configureView, SLOT(setIgnoreTimeEvents(bool)));
    connect(&engine, SIGNAL(inputPortAdded(int, QString)),
            &configureView, SLOT(addInputPort(int, QString)));
    connect(&engine, SIGNAL(inputPortChanged(int)),
            &configureView, SLOT(setInputPort(int)));
    connect(&engine, SIGNAL(inputPortChanged(int)),
            SLOT(handleDriverChange()));
    connect(&engine, SIGNAL(inputPortRemoved(int)),
            &configureView, SLOT(removeInputPort(int)));
    connect(&engine, SIGNAL(outputPortAdded(int, QString)),
            &configureView, SLOT(addOutputPort(int, QString)));
    connect(&engine, SIGNAL(outputPortChanged(int)),
            &configureView, SLOT(setOutputPort(int)));
    connect(&engine, SIGNAL(outputPortChanged(int)),
            SLOT(handleDriverChange()));
    connect(&engine, SIGNAL(outputPortRemoved(int)),
            &configureView, SLOT(removeOutputPort(int)));

    // Setup application
    connect(&application, SIGNAL(eventError(QString)),
            &errorView, SLOT(setMessage(QString)));
    connect(&application, SIGNAL(eventError(QString)),
            &errorView, SLOT(show()));
}

Controller::~Controller()
{
    // Disconnect engine signals handled by the controller before the engine is
    // deleted.
    disconnect(&engine, SIGNAL(messageReceived(quint64, const QByteArray &)),
               this, SLOT(handleReceivedMessage(quint64, const QByteArray &)));
    disconnect(&engine, SIGNAL(driverChanged(int)),
               this, SLOT(handleDriverChange()));
    disconnect(&engine, SIGNAL(inputPortChanged(int)),
               this, SLOT(handleDriverChange()));
    disconnect(&engine, SIGNAL(outputPortChanged(int)),
               this, SLOT(handleDriverChange()));
}

QString
Controller::getGenericDataDescription(const QByteArray &message, int lastIndex)
{
    assert((lastIndex >= -1) && (lastIndex < message.count()));
    if (lastIndex == -1) {
        lastIndex = message.count() - 1;
    }

    QStringList dataParts;
    for (int i = 1; i <= lastIndex; i++) {
        dataParts += QString("%1").
            arg(static_cast<uint>(message[i]), 2, 16, QChar('0'));
    }
    dataParts += tr("(%1 bytes)").arg(lastIndex);
    return dataParts.join(" ");
}

void
Controller::handleDriverChange()
{
    mainView.setMessageSendEnabled((engine.getDriver() != -1) &&
                                   (engine.getOutputPort() != -1));
}

void
Controller::handleMessageSend(const QString &message)
{
    // Convert the message to bytes.
    QStringList bytes = message.split(' ', QString::SkipEmptyParts);
    int count = bytes.count();
    QByteArray msg;
    for (int i = 0; i < count; i++) {
        QString byteStr = bytes[i];
        bool success;
        uint value = byteStr.toUInt(&success, 16);
        if ((! success) || (value > 0xff)) {
            showError(tr("'%1' is not a valid hexadecimal MIDI byte").
                      arg(byteStr));
            return;
        }
        msg.append(static_cast<char>(static_cast<quint8>(value)));
    }

    // Make sure the bytes represent a valid MIDI message.
    parseMessage(msg);
    if (! valid) {
        showError(tr("The given message is not a valid MIDI message."));
        return;
    }

    // Send the message.
    quint64 timeStamp = engine.sendMessage(msg);
    mainView.addSentMessage(timeStamp, statusDescription, dataDescription,
                            valid);
}

void
Controller::handleReceivedMessage(quint64 timeStamp, const QByteArray &message)
{
    parseMessage(message);
    mainView.addReceivedMessage(timeStamp, statusDescription, dataDescription,
                                valid);
}

void
Controller::parseMessage(const QByteArray &message)
{
    // Make sure we have an actual message.
    int length = message.count();
    if (! length) {
        dataDescription = "";
        statusDescription = tr("empty message");
        valid = false;
        return;
    }

    // Validate status byte.
    quint8 status = static_cast<quint8>(message[0]);
    if (status < 0x80) {
        dataDescription = getGenericDataDescription(message);
        statusDescription = tr("%1 (invalid status)").
            arg(static_cast<uint>(status), 2, 16, QChar('0'));
        valid = false;
        return;
    }

    // Validate length and data.
    QString errorMessage;
    int expectedLength = statusLengths[status - 0x80];
    int lastDataIndex;
    switch (expectedLength) {

    case -1:
        dataDescription = getGenericDataDescription(message);
        statusDescription = tr("%1 (undefined status)").
            arg(static_cast<uint>(status), 2, 16, QChar('0'));
        valid = false;
        return;

    case 0:
        if (length == 1) {
            dataDescription = "";
            statusDescription = tr("System Exclusive (no data)");
            valid = false;
            return;
        }
        if (static_cast<quint8>(message[length - 1]) != 0xf7) {
            dataDescription = getGenericDataDescription(message);
            statusDescription = tr("System Exclusive (end not found)");
            valid = false;
            return;
        }
        lastDataIndex = length - 2;
        break;

    default:
        if (length != expectedLength) {
            dataDescription = getGenericDataDescription(message);
            statusDescription = tr("%1 (incorrect length)").
                arg(static_cast<uint>(status), 2, 16, QChar('0'));
            valid = false;
            return;
        }
        lastDataIndex = length - 1;
    }

    // Validate data bytes.
    for (int i = 1; i <= lastDataIndex; i++) {
        if (static_cast<quint8>(message[i]) >= 0x80) {
            dataDescription = getGenericDataDescription(message);
            statusDescription = tr("%1 (invalid data)").
                arg(static_cast<uint>(status), 2, 16, QChar('0'));
            valid = false;
            return;
        }
    }

    // Convert message to user-friendly strings.
    QString s;
    int value;
    valid = true;
    switch (status & 0xf0) {

    case 0x80:
        dataDescription = tr("Note: %1, Velocity: %2").
            arg(getMIDINoteString(static_cast<quint8>(message[1]))).
            arg(static_cast<quint8>(message[2]));
        statusDescription = tr("Note Off, Channel %1").arg((status & 0xf) + 1);
        break;

    case 0x90:
        dataDescription = tr("Note: %1, Velocity: %2").
            arg(getMIDINoteString(static_cast<quint8>(message[1]))).
            arg(static_cast<quint8>(message[2]));
        statusDescription = tr("Note On, Channel %1").arg((status & 0xf) + 1);
        break;

    case 0xa0:
        dataDescription = tr("Note: %1, Pressure: %2").
            arg(getMIDINoteString(static_cast<quint8>(message[1]))).
            arg(static_cast<quint8>(message[2]));
        statusDescription = tr("Aftertouch, Channel %1").
            arg((status & 0xf) + 1);
        break;

    case 0xb0:
        dataDescription = tr("Controller: %1, Value: %2").
            arg(getMIDIControlString(static_cast<quint8>(message[1]))).
            arg(static_cast<quint8>(message[2]));
        statusDescription = tr("Controller, Channel %1").
            arg((status & 0xf) + 1);
        break;

    case 0xc0:
        dataDescription = tr("Number: %1").arg(static_cast<quint8>(message[1]));
        statusDescription = tr("Program Change, Channel %1").
            arg((status & 0xf) + 1);
        break;

    case 0xd0:
        dataDescription = tr("Pressure: %1").
            arg(static_cast<quint8>(message[1]));
        statusDescription = tr("Channel Pressure, Channel %1").
            arg((status & 0xf) + 1);
        break;

    case 0xe0:
        dataDescription = tr("Value: %1").
            arg((((static_cast<qint16>(message[2])) << 7) |
                 (static_cast<qint16>(message[1]))) - 0x2000);
        statusDescription = tr("Pitch Wheel, Channel %1").
            arg((status & 0xf) + 1);
        break;

    case 0xf0:
        switch (status & 0xf) {

        case 0x0:
            dataDescription = getGenericDataDescription(message, lastDataIndex);
            statusDescription = tr("System Exclusive");
            break;

        case 0x1:
            value = message[0] & 0xf;
            switch (message[0] & 0x70) {

            case 0x00:
                dataDescription = tr("Frames Low Nibble: %1").arg(value);
                break;

            case 0x10:
                dataDescription = tr("Frames High Nibble: %1").arg(value);
                break;

            case 0x20:
                dataDescription = tr("Seconds Low Nibble: %1").arg(value);
                break;

            case 0x30:
                dataDescription = tr("Seconds High Nibble: %1").arg(value);
                break;

            case 0x40:
                dataDescription = tr("Minutes Low Nibble: %1").arg(value);
                break;

            case 0x50:
                dataDescription = tr("Minutes High Nibble: %1").arg(value);
                break;

            case 0x60:
                dataDescription = tr("Hours Low Nibble: %1").arg(value);
                break;

            case 0x70:
                switch ((value & 0x6) >> 1) {

                case 0:
                    s = tr("24 fps");
                    break;

                case 1:
                    s = tr("25 fps");
                    break;

                case 2:
                    s = tr("30 fps (drop-frame)");
                    break;

                case 3:
                    s = tr("30 fps");
                    break;

                default:
                    // We shouldn't get here.
                    assert(false);
                }
                dataDescription = tr("Hours High Nibble: %1, SMPTE Type: %2").
                    arg(value & 1).arg(s);
                break;

            default:
                // We shouldn't get here.
                assert(false);
            }
            statusDescription = tr("MTC Quarter Frame");
            break;

        case 0x2:
            dataDescription = tr("MIDI Beat: %1").
                arg(((static_cast<qint16>(message[2])) << 7) |
                    (static_cast<qint16>(message[1])));
            statusDescription = tr("Song Position Pointer");
            break;

        case 0x3:
            dataDescription = tr("Song Number: %1").
                arg(static_cast<quint8>(message[1]));
            statusDescription = tr("Song Select");
            break;

        case 0x6:
            dataDescription = "";
            statusDescription = tr("Tune Request");
            break;

        case 0x8:
            dataDescription = "";
            statusDescription = tr("MIDI Clock");
            break;

        case 0x9:
            dataDescription = "";
            statusDescription = tr("MIDI Tick");
            break;

        case 0xa:
            dataDescription = "";
            statusDescription = tr("MIDI Start");
            break;

        case 0xb:
            dataDescription = "";
            statusDescription = tr("MIDI Continue");
            break;

        case 0xc:
            dataDescription = "";
            statusDescription = tr("MIDI Stop");
            break;

        case 0xe:
            dataDescription = "";
            statusDescription = tr("Active Sense");
            break;

        case 0xf:
            dataDescription = "";
            statusDescription = tr("Reset");
            break;

        default:
            // We shouldn't get here.
            assert(false);
        }
        break;

    default:
        // We shouldn't get here.
        assert(false);
    }
}

void
Controller::run()
{
    mainView.show();
    application.exec();
}

void
Controller::showError(const QString &message)
{
    errorView.setMessage(message);
    errorView.show();
}
