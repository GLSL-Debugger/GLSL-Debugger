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

#include <cmath>

#include <QtGui/QFrame>
#include <QtGui/QMouseEvent>
#include <QtGui/QBitmap>
#include <QtGui/QPainter>

#include "imageView.qt.h"

ImageView::ImageView(QWidget *parent) :
		QLabel(parent)
{
	setMouseTracking(true);
	m_minMaxLens = new QRubberBand(QRubberBand::Rectangle, this);
	m_rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
	m_zoomLevel = 1.0f;
	setMouseMode(MM_PICK);
	//setFocusPolicy(Qt::StrongFocus);
	//setFocus();
}

void ImageView::setImage(QImage &image)
{
	m_image = image;
	setPixmap(
			QPixmap::fromImage(
					image.scaled(m_zoomLevel * image.width(),
							m_zoomLevel * image.height())));
	resize(m_zoomLevel * image.width(), m_zoomLevel * image.height());
}

void ImageView::keyPressEvent(QKeyEvent *event)
{
	switch (m_mouseMode) {
	case MM_ZOOM:
		if (event->key() == Qt::Key_Control) {
			setCustomCursor(":/cursors/cursors/zoom-out.png");
		} else if (event->key() == Qt::Key_Shift) {
			setCustomCursor(":/cursors/cursors/zoom-reset.png");
		}
		break;
	default:
		event->ignore();
		break;
	}
}

void ImageView::keyReleaseEvent(QKeyEvent *event)
{
	switch (m_mouseMode) {
	case MM_ZOOM:
		if ((event->key() == Qt::Key_Control)
				|| (event->key() == Qt::Key_Shift)) {
			setCustomCursor(":/cursors/cursors/zoom-in.png");
		}
		break;
	default:
		event->ignore();
		break;
	}
}

void ImageView::mouseMoveEvent(QMouseEvent *event)
{
	/* this is really ugly! But as far as I can see the only way to ensure that
	 * the ImageView always has focus when the window is in the foreground
	 */
	if (m_pWorkspace->activeWindow()
			== parent()->parent()->parent()->parent()) {
		setFocus(Qt::MouseFocusReason);
	}
	if (visibleRegion().contains(event->pos())) {
		int x = event->x();
		int y = event->y();
		this->canvasToImage(x, y);
		emit mousePosChanged(x, y);
	} else {
		emit mousePosChanged(-1, -1);
	}

	switch (m_mouseMode) {
	case MM_ZOOM:
		if (event->modifiers() & Qt::ControlModifier) {
			m_rubberBand->hide();
		} else if (m_rubberBand->isVisible()) {
			m_rubberBand->setGeometry(
					QRect(m_rubberBandOrigin, event->pos()).normalized());
		}
		break;
	case MM_MINMAX:
		if ((event->buttons() & Qt::LeftButton) && m_minMaxLens->isVisible()) {
			m_minMaxLens->setGeometry(
					QRect(m_minMaxLensOrigin, event->pos()).normalized());
		}
		break;
	default:
		break;
	}
}

void ImageView::mousePressEvent(QMouseEvent *event)
{
	switch (m_mouseMode) {
	case MM_ZOOM:
		m_rubberBand->setGeometry(0, 0, 0, 0);
		if (event->modifiers() & Qt::ControlModifier) {
			m_rubberBand->hide();
		} else {
			m_rubberBandOrigin = event->pos();
			m_rubberBand->show();
		}
		break;
	case MM_MINMAX:
		m_minMaxLens->show();
		//this->repaint(this->m_minMaxLens->geometry());
		this->repaint(0, 0, -1, -1);
		this->m_minMaxLens->setGeometry(QRect());
		m_minMaxLensOrigin = event->pos();
		break;
	case MM_PICK:
		if (visibleRegion().contains(event->pos())) {
			int x = event->x();
			int y = event->y();
			this->canvasToImage(x, y);
			emit picked(x, y);
		}
		break;
	default:
		break;
	}
}

void ImageView::mouseReleaseEvent(QMouseEvent *event)
{
	switch (m_mouseMode) {
	case MM_ZOOM:
		m_rubberBand->hide();
		if (m_rubberBand->geometry().isNull()) {
			/* Zoom step. */
			int newCenterX, newCenterY;
			int x = event->x();
			int y = event->y();

			if (event->modifiers() & Qt::ControlModifier) {
				this->zoomOut();
				if (m_zoomLevel != 1) {
					newCenterX = (int) (x * (this->m_zoomLevel - 1.0)
							/ this->m_zoomLevel);
					newCenterY = (int) (y * (this->m_zoomLevel - 1.0)
							/ this->m_zoomLevel);
					emit viewCenterChanged(newCenterX, newCenterY);
					if (visibleRegion().contains(event->pos())) {
						this->canvasToImage(x, y);
						emit mousePosChanged(x, y);
					} else {
						emit mousePosChanged(-1, -1);
					}
				}
			} else if (event->modifiers() == Qt::ShiftModifier) {
				this->setZoomLevel(1.0f);
			} else {
				this->zoomIn();
				newCenterX = (int) (x * (this->m_zoomLevel + 1.0)
						/ this->m_zoomLevel);
				newCenterY = (int) (y * (this->m_zoomLevel + 1.0)
						/ this->m_zoomLevel);
				emit viewCenterChanged(newCenterX, newCenterY);
				if (visibleRegion().contains(event->pos())) {
					this->canvasToImage(x, y);
					emit mousePosChanged(x, y);
				} else {
					emit mousePosChanged(-1, -1);
				}
			}

		} else {
			/* Lasso zoom. */
			if (!(event->modifiers() & Qt::ControlModifier)) {
				zoomRegion(m_rubberBand->geometry());
			}
		}
		break;
	case MM_MINMAX: {
		QRect selRect = this->m_minMaxLens->geometry();
		//QRect visRect = this->visibleRegion().boundingRect();
		selRect.moveLeft(selRect.left() / this->m_zoomLevel);
		selRect.moveTop(selRect.top() / this->m_zoomLevel);
		selRect.setWidth(
				::ceilf(
						static_cast<float>(selRect.width())
								/ this->m_zoomLevel));
		selRect.setHeight(
				::ceilf(
						static_cast<float>(selRect.height())
								/ this->m_zoomLevel));
		emit minMaxAreaChanged(selRect);
		this->m_minMaxLens->setVisible(false);
		if (event->modifiers() & Qt::ControlModifier) {
			emit setMappingBounds();
		}
	}
		break;
	default:
		break;
	}
}

void ImageView::paintEvent(QPaintEvent *evt)
{
	QLabel::paintEvent(evt);

	QRect minMaxRect = this->m_minMaxLens->geometry();

	if (!this->m_minMaxLens->isVisible() && !minMaxRect.isEmpty()) {
		QPainter painter(this);
		painter.setRenderHint(QPainter::Antialiasing);
		painter.setBrush(Qt::NoBrush);
		painter.setPen(
				QPen(this->palette().color(QPalette::Highlight), 1.0,
						Qt::DashLine));

		painter.drawRect(minMaxRect);
	}

}

void ImageView::zoomIn()
{
	this->setZoomLevel(this->m_zoomLevel + 1.0f);
}

void ImageView::zoomOut()
{
	if ((this->m_zoomLevel -= 1.0f) < 1.0f) {
		this->m_zoomLevel = 1.0f;
	}

	this->setZoomLevel(this->m_zoomLevel);
}

void ImageView::zoomRegion(const QRect& region)
{
	QRect size = this->geometry();                              // Canvas size.
	QRect targetRect = region.intersected(size);                // Zoom region.
	QPoint targetCenter = targetRect.center();                  // Zoom center.
	QRect visibleRect = this->visibleRegion().boundingRect();   // Viewport.

	float sx = static_cast<float>(visibleRect.width()) / targetRect.width();
	float sy = static_cast<float>(visibleRect.height()) / targetRect.height();
	float newScale = (sx < sy) ? sx : sy;
	float scale = newScale * static_cast<float>(size.width()) / m_image.width();

	this->setZoomLevel(scale);

	emit viewCenterChanged(newScale * targetCenter.x(),
			newScale * targetCenter.y());
}

void ImageView::setZoomLevel(const float zoomLevel)
{
	this->m_zoomLevel = zoomLevel;

	int newWidth = static_cast<int>(this->m_zoomLevel * m_image.width());
	int newHeight = static_cast<int>(this->m_zoomLevel * m_image.height());

	this->resize(newWidth, newHeight);
	this->setPixmap(QPixmap::fromImage(m_image.scaled(newWidth, newHeight)));
}

void ImageView::canvasToImage(int& inOutX, int& inOutY)
{
	inOutX = static_cast<float>(inOutX) / this->m_zoomLevel;
	inOutY = static_cast<float>(inOutY) / this->m_zoomLevel;
}

void ImageView::setCustomCursor(const char *name)
{
	QPixmap cursor(QString::fromUtf8(name));
	setCursor(QCursor(cursor));
}

void ImageView::setMouseMode(int mouseMode)
{
	m_mouseMode = mouseMode;
	switch (m_mouseMode) {
	case MM_ZOOM:
		setCustomCursor(":/cursors/cursors/zoom-in.png");
		break;
	case MM_PICK:
		setCustomCursor(":/cursors/cursors/pick.png");
		break;
	case MM_MINMAX:
		setCustomCursor(":/cursors/cursors/min-max.png");
		break;
	default:
		break;
	}

	if (m_mouseMode != MM_MINMAX) {
		this->m_minMaxLens->setGeometry(QRect());
		this->m_minMaxLens->setVisible(false);
		emit minMaxAreaChanged(this->m_minMaxLens->geometry());
		this->repaint(0, 0, -1, -1);
	}
}

void ImageView::setWorkspace(QWorkspace *ws)
{
	m_pWorkspace = ws;
}
