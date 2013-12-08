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

#ifndef _IMAGEVIEW_QT_H
#define _IMAGEVIEW_QT_H

#include <QtGui/QLabel>
#include <QtGui/QImage>
#include <QtGui/QWidget>
#include <QtGui/QRubberBand>
#include <QtGui/QWorkspace>

class ImageView: public QLabel {
Q_OBJECT

public:
	enum MouseMode {
		MM_NONE,
		MM_ZOOM,
		MM_PICK,
		MM_MINMAX
	};

	ImageView(QWidget *parent = 0);
	void setImage(QImage &image);
	void setMouseMode(int mouseMode);
	void setWorkspace(QWorkspace *ws);

	QImage getImage(void)
	{
		return m_image;
	}

protected:
	virtual void keyPressEvent(QKeyEvent *event);
	virtual void keyReleaseEvent(QKeyEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void paintEvent(QPaintEvent *evt);

	void setCustomCursor(const char *name);

	void zoomIn();
	void zoomOut();
	void zoomRegion(const QRect &region);
	void setZoomLevel(const float zoomLevel);
	void canvasToImage(int& inOutX, int& inOutY);

signals:
	void mousePosChanged(int x, int y);
	void picked(int x, int y);
	void viewCenterChanged(int x, int y);
	void minMaxAreaChanged(const QRect& minMaxArea);
	void setMappingBounds();

protected:
	QRubberBand *m_minMaxLens;
	QRubberBand *m_rubberBand;
	QPoint m_minMaxLensOrigin;
	QPoint m_rubberBandOrigin;
	int m_mouseMode;
	float m_zoomLevel;
	int m_lastZoomEvent;
	QImage m_image;
	QWorkspace *m_pWorkspace;
};

#endif
