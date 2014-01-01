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

#include <QtGui/QTextFrame>
#include <QtWidgets/QApplication>
#include <QtWidgets/QTextEdit>

#include "messagetabledelegate.h"

MessageTableDelegate::MessageTableDelegate(QObject *parent):
    QStyledItemDelegate(parent)
{
    // Empty
}

MessageTableDelegate::~MessageTableDelegate()
{
    // Empty
}

QWidget *
MessageTableDelegate::createEditor(QWidget *parent,
                                   const QStyleOptionViewItem &/*option*/,
                                   const QModelIndex &/*index*/) const
{
    QTextEdit *editor = new QTextEdit(parent);

    editor->setFrameShadow(QTextEdit::Plain);
    editor->setFrameShape(QTextEdit::NoFrame);
    editor->setLineWidth(0);
    editor->setMidLineWidth(0);

    editor->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    editor->setReadOnly(true);
    editor->setTextInteractionFlags(Qt::TextSelectableByKeyboard |
                                    Qt::TextSelectableByMouse);
    return editor;
}

void
MessageTableDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    QTextEdit *textEditor = qobject_cast<QTextEdit *>(editor);
    textEditor->setPlainText(index.data(Qt::EditRole).toString());
    QTextDocument *document = textEditor->document();
    document->setDocumentMargin(0);
    QTextFrame *rootFrame = document->rootFrame();
    QTextFrameFormat frameFormat = rootFrame->frameFormat();
    frameFormat.setBottomMargin(0);
    frameFormat.setLeftMargin(3);
    frameFormat.setRightMargin(3);
    frameFormat.setTopMargin(0);
    rootFrame->setFrameFormat(frameFormat);
}

void
MessageTableDelegate::setModelData(QWidget */*editor*/,
                                   QAbstractItemModel */*model*/,
                                   const QModelIndex &/*index*/) const
{
    // Empty
}

void
MessageTableDelegate::updateEditorGeometry(QWidget *editor,
                                           const QStyleOptionViewItem &option,
                                           const QModelIndex &/*index*/) const
{
    editor->setGeometry(option.rect);
}
