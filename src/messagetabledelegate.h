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

#ifndef __MESSAGETABLEDELEGATE_H__
#define __MESSAGETABLEDELEGATE_H__

#include <QtWidgets/QStyledItemDelegate>

class MessageTableDelegate: public QStyledItemDelegate {

    Q_OBJECT

public:

    explicit
    MessageTableDelegate(QObject *parent=0);

    ~MessageTableDelegate();

    QWidget *
    createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                 const QModelIndex &index) const;

    void
    setEditorData(QWidget *editor, const QModelIndex &index) const;

    void
    setModelData(QWidget *editor, QAbstractItemModel *model,
                 const QModelIndex &index) const;

    void
    updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const;

};

#endif
