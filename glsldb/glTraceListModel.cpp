/******************************************************************************

Copyright (C) 2006-2009 Institute for Visualization and Interactive Systems
(VIS), Universität Stuttgart.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice, this
    list of conditions and the following disclaimer in the documentation and/or
    other materials provided with the distribution.

  * Neither the name of the name of VIS, Universität Stuttgart nor the names
    of its contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#include "glTraceListModel.qt.h"

#include <QtGui/QColor>
#include <QtGui/QBrush>
#include <QtGui/QFont>

GlTraceListItem::GlTraceListItem()
{

}

GlTraceListItem::~GlTraceListItem()
{

}

void GlTraceListItem::setIconType(IconType type)
{
    m_eIconType = type;

    switch(type) {
        case IT_EMPTY:
            m_qItem = QIcon(QString::fromUtf8(":/icons/icons/empty_32.png"));
            break;
        case IT_ACTUAL:
            m_qItem = QIcon(QString::fromUtf8(":/icons/icons/go-actual_32.png"));
            break;
        case IT_OK:
            m_qItem = QIcon(QString::fromUtf8(":/icons/icons/dialog-ok_32.png"));
            break;
        case IT_ERROR:
            m_qItem = QIcon(QString::fromUtf8(":/icons/icons/dialog-error_32.png"));
            break;
        case IT_WARNING:
            m_qItem = QIcon(QString::fromUtf8(":/icons/icons/dialog-warning_32.png"));
            break;
        case IT_IMPORTANT:
            m_qItem = QIcon(QString::fromUtf8(":/icons/icons/emblem-important_32.png"));
            break;
        case IT_RECORD:
            m_qItem = QIcon(QString::fromUtf8(":/icons/icons/media-record_32.png"));
            break;
        default:
            m_qItem = QIcon(QString::fromUtf8(":/icons/icons/empty_32.png"));
    }
}

GlTraceListFilterModel::GlTraceListFilterModel(GlTraceFilterModel *traceFilter, QObject *parent)
    : QSortFilterProxyModel(parent)
{
    m_GlTraceFilterModel = traceFilter;
    currentShown = false;
}

bool GlTraceListFilterModel::filterAcceptsRow(int sourceRow, const
                                                 QModelIndex &sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    QString text = sourceModel()->data(index, Qt::DisplayRole).toString();

    /*
    return (!(text.startsWith(QString("glXGetProcAddressARB("))));
    */
    //return true;
    return m_GlTraceFilterModel->isFunctionVisible(text.left(text.indexOf("(")))
        || dynamic_cast<GlTraceListModel*>(sourceModel())->isCurrentCall(index);
}

void GlTraceListFilterModel::showCurrentItemAnyway(bool show) {
    currentShown = show;
}

GlTraceListModel::GlTraceListModel(int maxListEntries, GlTraceFilterModel *traceFilter, QObject *parent)
    : QAbstractListModel(parent)
{
    m_iMax = maxListEntries;
    m_pData = new GlTraceListItem[maxListEntries];
    m_iFirst = 0;
    m_iNum   = 0;
    m_pTraceFilterModel = traceFilter;
}

GlTraceListModel::~GlTraceListModel()
{
    delete[] m_pData;
}

void GlTraceListModel::clear(void)
{
    beginResetModel();
    m_iFirst = 0;
    m_iNum   = 0;
    endResetModel();
}

int  GlTraceListModel::getNextIndex(void)
{
    int idx;

    m_iNum++;
    if (m_iNum > m_iMax) {
        m_iNum = m_iMax;
        beginRemoveRows(QModelIndex(), 0, 0);
        m_iFirst++;
        endRemoveRows();
        if (m_iFirst >= m_iMax) {
            m_iFirst = 0;
        }

        idx = m_iFirst - 1;
        if (idx < 0) {
            idx = m_iMax - 1;
        }

    } else {
        idx = m_iNum - 1;
    }

    return idx;
}

int GlTraceListModel::getLastRowIndex(void)
{
    return m_iNum - 1;
}

void GlTraceListModel::addGlTraceItem(const  GlTraceListItem::IconType type, const QString & text)
{
    int idx = getNextIndex();
    int newRow = getLastRowIndex();
    beginInsertRows(QModelIndex(), newRow, newRow);
    m_pData[idx].setIconType(type);
    m_pData[idx].setText(text);
    endInsertRows();
}

void GlTraceListModel::addGlTraceWarningItem(const QString & text)
{
    UNUSED_ARG(text)
    int idx = getNextIndex();
    int newRow = getLastRowIndex();
    beginInsertRows(QModelIndex(), newRow, newRow);
    m_pData[idx].setIconType(GlTraceListItem::IT_WARNING);
    endInsertRows();
}

void GlTraceListModel::addGlTraceErrorItem(const QString & text)
{
    UNUSED_ARG(text)
    int idx = getNextIndex();
    int newRow = getLastRowIndex();
    beginInsertRows(QModelIndex(), newRow, newRow);
    m_pData[idx].setIconType(GlTraceListItem::IT_ERROR);
    endInsertRows();
}

void GlTraceListModel::setCurrentGlTraceIconType(const GlTraceListItem::IconType type, int offset)
{
    int idx;
    int rowIdx = m_iNum + offset;

    idx = m_iFirst + rowIdx;
    if (idx >= m_iMax) {
        idx -= m_iMax;
    }

    if ( 0 <= idx && idx < m_iMax) {
        m_pData[idx].setIconType(type);
        QModelIndex dataIndex = createIndex(rowIdx, 0);
        dataChanged(dataIndex, dataIndex);
    }
}

void GlTraceListModel::setCurrentGlTraceText(const QString &text, int offset)
{
    int idx;
    int rowIdx = m_iNum + offset;

    idx = m_iFirst + rowIdx;
    if (idx >= m_iMax) {
        idx -= m_iMax;
    }

    if ( 0 <= idx && idx < m_iMax) {
        m_pData[idx].setText(text);
        QModelIndex dataIndex = createIndex(rowIdx, 0);
        dataChanged(dataIndex, dataIndex);
    }
}

int GlTraceListModel::rowCount(const QModelIndex &parent) const
{
    UNUSED_ARG(parent)
    return m_iNum;
}

QVariant GlTraceListModel::data(const QModelIndex &index, int role) const
{
    int idx;

    if (!index.isValid()) {
        return QVariant();
    }

    idx = m_iFirst + index.row();
    if (idx >= m_iMax) {
        idx -= m_iMax;
    }

    switch (role) {
        case Qt::ForegroundRole:
            if (m_pTraceFilterModel->isFunctionVisible(m_pData[idx].getText().left(m_pData[idx].getText().indexOf("(")))) {
                return QVariant();
            } else {
                return QBrush(QColor(128,128,128));
            }
        case Qt::FontRole:
            if (m_pTraceFilterModel->isFunctionVisible(m_pData[idx].getText().left(m_pData[idx].getText().indexOf("(")))) {
                return QVariant();
            } else {
                QFont f;
                f.setItalic(true);
                return f;
            }
        case Qt::DisplayRole:
            return m_pData[idx].getText();
        case Qt::DecorationRole:
            return m_pData[idx].getIcon();
        default:
            return QVariant();
    }
}

bool GlTraceListModel::isCurrentCall(const QModelIndex &index) {
    if ((m_iFirst + index.row()) % m_iMax == (m_iFirst + m_iNum - 1 + m_iMax) % m_iMax) {
        return true;
    } else {
        return false;
    }
}

void GlTraceListItem::outputTXT(QTextStream &out) const
{
    switch(m_eIconType) {
        case IT_EMPTY:
            out << QString("  ");
            break;
        case IT_ACTUAL:
            out << QString("> ");
            break;
        case IT_OK:
            out << QString("| ");
            break;
        case IT_ERROR:
            out << QString("E!");
            break;
        case IT_WARNING:
            out << QString("W!");
            break;
        case IT_IMPORTANT:
            out << QString("I ");
            break;
        case IT_RECORD:
            out << QString("* ");
            break;
        default:
            out << QString("? ");
    }
    out << " " << getText() << endl;
}

void GlTraceListModel::traverse(QTextStream &out, void (GlTraceListItem::*pt2Member)(QTextStream&) const)
{
    int i, idx;

    for (i = 0, idx=m_iFirst; i < m_iNum; i++, idx++) {
        if (idx >= m_iMax) {
            idx -= m_iMax;
        }
        if ( 0 <= idx && idx < m_iMax) {
            (m_pData[idx].*pt2Member)(out);
        }
    }
}

