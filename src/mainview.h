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

#ifndef __MAINVIEW_H__
#define __MAINVIEW_H__

#include <QtGui/QAction>
#include <QtGui/QMainWindow>
#include <QtGui/QStandardItemModel>
#include <QtGui/QTableView>

#include "designerview.h"
#include "messagetabledelegate.h"

class MainView: public DesignerView {

    Q_OBJECT

public:

    explicit
    MainView(QObject *parent=0);

    ~MainView();

public slots:

    void
    addReceivedMessage(quint64 timeStamp, const QString &statusDescription,
                       const QString &dataDescription, bool valid);

    void
    addSentMessage(quint64 timeStamp, const QString &statusDescription,
                   const QString &dataDescription, bool valid);

    void
    clearMessages();

    void
    setMessageSendEnabled(bool enabled);

signals:

    void
    aboutRequest();

    void
    addMessageRequest();

    void
    clearMessagesRequest();

    void
    configureRequest();

private:

    enum MessageTableColumn {
        MESSAGETABLECOLUMN_TIMESTAMP = 0,
        MESSAGETABLECOLUMN_STATUS = 1,
        MESSAGETABLECOLUMN_DATA = 2,

        MESSAGETABLECOLUMN_TOTAL = 3
    };

    int
    addMessage(quint64 timeStamp, const QString &statusDescription,
               const QString &dataDescription, bool valid);

    void
    setModelData(int row, int column, const QVariant &value,
                 int role=Qt::DisplayRole);

    QAction *aboutAction;
    QAction *addAction;
    QAction *clearAction;
    QAction *configureAction;
    MessageTableDelegate tableDelegate;
    QAction *quitAction;
    QStandardItemModel tableModel;
    QTableView *tableView;

};

#endif
