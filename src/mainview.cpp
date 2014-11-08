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

#include <QtWidgets/QApplication>

#include "mainview.h"
#include "util.h"

MainView::MainView(QObject *parent):
    DesignerView(":/midisnoop/mainview.ui", parent)
{
    QWidget *widget = getRootWidget();

    aboutAction = getChild<QAction>(widget, "aboutAction");
    connect(aboutAction, SIGNAL(triggered()),
            SIGNAL(aboutRequest()));

    addAction = getChild<QAction>(widget, "addAction");
    connect(addAction, SIGNAL(triggered()),
            SIGNAL(addMessageRequest()));

    clearAction = getChild<QAction>(widget, "clearAction");
    connect(clearAction, SIGNAL(triggered()),
            SIGNAL(clearMessagesRequest()));

    configureAction = getChild<QAction>(widget, "configureAction");
    connect(configureAction, SIGNAL(triggered()),
            SIGNAL(configureRequest()));

    quitAction = getChild<QAction>(widget, "quitAction");
    connect(quitAction, SIGNAL(triggered()),
            SIGNAL(closeRequest()));

    tableModel.setColumnCount(MESSAGETABLECOLUMN_TOTAL);
    tableModel.setRowCount(0);
    tableModel.setHeaderData(MESSAGETABLECOLUMN_DATA, Qt::Horizontal,
                             tr("Data"), Qt::DisplayRole);
    tableModel.setHeaderData(MESSAGETABLECOLUMN_STATUS, Qt::Horizontal,
                             tr("Status"), Qt::DisplayRole);
    tableModel.setHeaderData(MESSAGETABLECOLUMN_TIMESTAMP, Qt::Horizontal,
                             tr("Timestamp"), Qt::DisplayRole);

    tableView = getChild<QTableView>(widget, "centralWidget");
    tableView->setItemDelegate(&tableDelegate);
    tableView->setModel(&tableModel);
}

MainView::~MainView()
{
    // Empty
}

int
MainView::addMessage(quint64 timeStamp, const QString &statusDescription,
                     const QString &dataDescription, bool valid)
{
    int count = tableModel.rowCount();
    bool inserted = tableModel.insertRow(count);
    assert(inserted);
    Qt::AlignmentFlag alignment = Qt::AlignTop;
    setModelData(count, MESSAGETABLECOLUMN_DATA, dataDescription);
    setModelData(count, MESSAGETABLECOLUMN_DATA, alignment,
                 Qt::TextAlignmentRole);
    setModelData(count, MESSAGETABLECOLUMN_STATUS, statusDescription);
    setModelData(count, MESSAGETABLECOLUMN_STATUS, alignment,
                 Qt::TextAlignmentRole);
    setModelData(count, MESSAGETABLECOLUMN_TIMESTAMP, timeStamp);
    setModelData(count, MESSAGETABLECOLUMN_TIMESTAMP, alignment,
                 Qt::TextAlignmentRole);
    if (! valid) {
        setModelData(count, MESSAGETABLECOLUMN_STATUS,
                     QIcon(":/midisnoop/images/16x16/error.png"),
                     Qt::DecorationRole);
    }
    tableView->resizeRowToContents(count);
    tableView->scrollToBottom();
    return count;
}

void
MainView::addReceivedMessage(quint64 timeStamp,
                             const QString &statusDescription,
                             const QString &dataDescription, bool valid)
{
    addMessage(timeStamp, statusDescription, dataDescription, valid);
}

void
MainView::addSentMessage(quint64 timeStamp, const QString &statusDescription,
                         const QString &dataDescription, bool valid)
{
    int index = addMessage(timeStamp, statusDescription, dataDescription,
                           valid);
    const QBrush &brush = qApp->palette().alternateBase();
    setModelData(index, MESSAGETABLECOLUMN_DATA, brush, Qt::BackgroundRole);
    setModelData(index, MESSAGETABLECOLUMN_STATUS, brush, Qt::BackgroundRole);
    setModelData(index, MESSAGETABLECOLUMN_TIMESTAMP, brush,
                 Qt::BackgroundRole);
}

void
MainView::clearMessages()
{
    int count = tableModel.rowCount();
    if (count) {
        bool removed = tableModel.removeRows(0, tableModel.rowCount());
        assert(removed);
    }
}

void
MainView::setMessageSendEnabled(bool enabled)
{
    addAction->setEnabled(enabled);
}

void
MainView::setModelData(int row, int column, const QVariant &value, int role)
{
    bool result = tableModel.setData(tableModel.index(row, column), value,
                                     role);
    assert(result);
}
