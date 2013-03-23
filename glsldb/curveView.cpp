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

#include "curveView.qt.h"
#include "colors.qt.h"
#include <math.h>
#include <QtGui/QPen>
#include <QtGui/QPainter>

#ifdef _WIN32
/* C99 sucks */
inline static int round(double x) {
	int y;
	y = (int)(x + 0.5);
	while ((double)y < x - 0.5) y++;
	while ((double)y > x + 0.5) y--;
	return y;
}
#endif /* _WIN32 */

CurveView::CurveView(QWidget *parent)
    : QAbstractItemView(parent)
{
    horizontalScrollBar()->setRange(0, 0);
    verticalScrollBar()->setRange(0, 0);

    /* Sizes */
    m_nMargin             = 10;
    m_nLabelSize[0]       = 75;
    m_nLabelSize[1]       = 15;
    m_nTickSize[0]        = 64;
    m_nTickSize[1]        = 2;
    m_nDrawAreaMinimum[0] = 256;
    m_nDrawAreaMinimum[1] = 100;
    m_nAxisArrow[0]       = 3;
    m_nAxisArrow[1]       = 3;
    m_nTickSpace[0]       = 50;
    m_nTickSpace[1]       = 20;

    m_nBase = -1;
    
    updateGeometries();
}

void CurveView::setBase(int base)
{
    m_nBase = 0;
    viewport()->update();
}

void CurveView::addMapping(int col, QColor color)
{
    m_qMapping[col] = color;
    viewport()->update();
}

void CurveView::delMapping(int col)
{
    m_qMapping.remove(col);
    viewport()->update();
}


void CurveView::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    QAbstractItemView::dataChanged(topLeft, bottomRight);

    viewport()->update();
}

void CurveView::rowsInserted(const QModelIndex &parent, int start, int end)
{
    QAbstractItemView::rowsInserted(parent, start, end);

    viewport()->update();
}


QRect CurveView::visualRect(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QRect();
    }

    // Check if we shoud care about this
    if (index.column() != m_nBase &&
        !m_qMapping.contains(index.column())) {
        return QRect();
    } else {
        return viewport()->rect();
    }
}

void CurveView::scrollTo(const QModelIndex &index, ScrollHint hint)
{

}

QModelIndex CurveView::indexAt(const QPoint &point) const
{
    return QModelIndex();
}

QModelIndex CurveView::moveCursor(QAbstractItemView::CursorAction cursorAction,
                                  Qt::KeyboardModifiers modifiers)
{
    return QModelIndex();
}
    
int CurveView::horizontalOffset() const
{
    return horizontalScrollBar()->value();
}

int CurveView::verticalOffset() const
{
    return verticalScrollBar()->value();
}
    
bool CurveView::isIndexHidden(const QModelIndex &index) const
{
    return false;
}

void CurveView::setSelection(const QRect&, QItemSelectionModel::SelectionFlags command)
{

}
    
QRegion CurveView::visualRegionForSelection(const QItemSelection &selection) const
{
    return QRegion();
}

void CurveView::paintEvent(QPaintEvent *event)
{
    int i, tx, ty;

    if (m_qMapping.count() == 0 || m_nBase == -1) {
        return;
    }

    QStyleOptionViewItem option = viewOptions();
    QStyle::State state = option.state;

    QBrush background = option.palette.base();
    QPen foreground(option.palette.color(QPalette::WindowText));
    QPen textPen(option.palette.color(QPalette::Text));
    QPen highlightedPen(option.palette.color(QPalette::HighlightedText));

    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(event->rect(), background);
    painter.setPen(foreground);

    QRect dataRect = QRect(m_nMargin + m_nLabelSize[0], 
                           m_nMargin, 
                           m_nDrawArea[0], 
                           m_nDrawArea[1]);

    painter.save();
    painter.translate(dataRect.x() - horizontalScrollBar()->value(),
                      dataRect.y() - verticalScrollBar()->value());

    QModelIndex lastIndex = model()->index(model()->rowCount()-1, m_nBase, rootIndex());
    int numIterations = model()->data(lastIndex).toInt();
    
    /* x-axis */
    painter.drawLine(0, m_nDrawArea[1], m_nDrawArea[0] + m_nAxisArrow[0], m_nDrawArea[1]);
    painter.drawLine(m_nDrawArea[0], m_nDrawArea[1] + m_nAxisArrow[1],
                     m_nDrawArea[0] + m_nAxisArrow[0], m_nDrawArea[1]);
    painter.drawLine(m_nDrawArea[0], m_nDrawArea[1] - m_nAxisArrow[1],
                     m_nDrawArea[0] + m_nAxisArrow[0], m_nDrawArea[1]);

    /* y-axis */
    painter.drawLine(0, -m_nAxisArrow[1], 0, m_nDrawArea[1]);
    painter.drawLine(-m_nAxisArrow[1], 0, 0, -m_nAxisArrow[1]);
    painter.drawLine( m_nAxisArrow[1], 0, 0, -m_nAxisArrow[1]);

    /* max data value */
    int maxValue = 0;
    for (int row = 0; row < model()->rowCount(rootIndex()); ++row) {
        QMap<int, QColor>::const_iterator iter = m_qMapping.constBegin();
        while (iter != m_qMapping.constEnd()) {
            QModelIndex index = model()->index(row, iter.key(), rootIndex());
            int value = model()->data(index).toInt();
            if (value > maxValue) {
                maxValue = value;
            }
            ++iter;
        }
    }

    if (maxValue == 0 || numIterations == 0) {
        painter.restore();
        return;
    }
    
    /* x-ticks */
    tx = (int) floor(m_nDrawArea[0]/(float)m_nTickSpace[0]);
    int iterPerTickX = (int) ceil(numIterations / (float) tx);

    for (i=0; i<=numIterations; i += iterPerTickX) {
        int dx = (int) round((m_nDrawArea[0]*i)/(float)numIterations);
        painter.drawLine(dx, m_nDrawArea[1] - m_nTickSize[1], dx, m_nDrawArea[1] + m_nTickSize[1]);
        painter.drawText(dx, m_nDrawArea[1]+m_nLabelSize[1], QString::number(i));
    }


    /* y-ticks */
    ty = (int) floor(m_nDrawArea[1]/(float)m_nTickSpace[1]);
    int iterPerTickY = (int) ceil(maxValue / (float) ty);
    
    for (i=0; i<=maxValue; i += iterPerTickY) {
        int dy = (int) round((m_nDrawArea[1]*i)/(float)maxValue);
        
        painter.drawLine(-m_nTickSize[1], m_nDrawArea[1]-dy, m_nTickSize[1], m_nDrawArea[1]-dy);
        painter.drawText(-m_nLabelSize[0]-2*m_nTickSize[1], m_nDrawArea[1]-dy-m_nLabelSize[1]/2,
                m_nLabelSize[0], m_nLabelSize[1], Qt::AlignRight|Qt::AlignVCenter,
                QString::number(i));
    }


    /* curves */
    int xOld = -10;
    int yOld = 0;
    QMap<int, QColor>::const_iterator iter = m_qMapping.constBegin();
    while (iter != m_qMapping.constEnd()) {
        painter.save();
        painter.setPen(iter.value());
        
        for (int row = 0; row < model()->rowCount(rootIndex()); ++row) {
            QModelIndex index = model()->index(row, m_nBase, rootIndex());
            int xValue = model()->data(index).toInt();
            
            index = model()->index(row, iter.key(), rootIndex());
            int yValue = model()->data(index).toInt();
            
            if (xValue == xOld+1) {
                painter.drawLine((m_nDrawArea[0]*xOld)/(numIterations),
                        m_nDrawArea[1]-(m_nDrawArea[1]*yOld)/maxValue,
                        (m_nDrawArea[0]*xValue)/(numIterations),
                        m_nDrawArea[1]-(m_nDrawArea[1]*yValue)/maxValue);
            } else {
                painter.drawPoint((m_nDrawArea[0]*xValue)/(numIterations), 
                        m_nDrawArea[1]-(m_nDrawArea[1]*yValue)/maxValue);
            }
            xOld = xValue;
            yOld = yValue;
        }
        
        painter.restore();
        ++iter;
    }
        
    painter.restore();
}
    
void CurveView::updateGeometries()
{

    /* Update width sizes */
    m_nTotalSize[0] = m_nDrawAreaMinimum[0] + 2 * m_nLabelSize[0] + m_nTickSize[1] + 2*m_nMargin;
    if ( m_nTotalSize[0] < viewport()->width()) {
        m_nTotalSize[0] = viewport()->width();
        m_nDrawArea[0]  = m_nTotalSize[0] - 2 * m_nLabelSize[0] - m_nTickSize[1] - 2*m_nMargin;
    } else {
        m_nDrawArea[0] = m_nDrawAreaMinimum[0];
    }

    /* Updat height sizes */
    m_nTotalSize[1] = m_nDrawAreaMinimum[1] + m_nLabelSize[1] + m_nTickSize[1] + 2*m_nMargin;
    if ( m_nTotalSize[1] < viewport()->height()) {
        m_nTotalSize[1] = viewport()->height();
        m_nDrawArea[1]  = m_nTotalSize[1] - m_nLabelSize[1] - m_nTickSize[1] - 2*m_nMargin;
    } else {
        m_nDrawArea[1] = m_nDrawAreaMinimum[1];
    }
    
    horizontalScrollBar()->setPageStep(viewport()->width());
    horizontalScrollBar()->setRange(0, qMax(0, m_nTotalSize[0] - viewport()->width()));
    verticalScrollBar()->setPageStep(viewport()->height());
    verticalScrollBar()->setRange(0, qMax(0, m_nTotalSize[1] - viewport()->height()));
}

void CurveView::resizeEvent(QResizeEvent *event)
{
    updateGeometries();
}

void CurveView::scrollContentsBy(int dx, int dy)
{
    viewport()->scroll(dx, dy);
}

