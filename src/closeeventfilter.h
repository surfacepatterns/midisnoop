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

#ifndef __CLOSEEVENTFILTER_H__
#define __CLOSEEVENTFILTER_H__

#include <QtCore/QEvent>
#include <QtCore/QObject>

class CloseEventFilter: public QObject {

    Q_OBJECT

public:

    explicit
    CloseEventFilter(QObject *parent=0);

    ~CloseEventFilter();

    bool
    eventFilter(QObject *obj, QEvent *event);

    bool
    isCloseEnabled() const;

    void
    setCloseEnabled(bool enabled);

signals:

    void
    closeRequest();

private:

    bool closeEnabled;

};

#endif
