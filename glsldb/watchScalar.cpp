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

#include "watchScalar.qt.h"
#include <QtGui/QGridLayout>

#include "dbgprint.h"

#define CLAMP(x, min, max) ((x) < (min) ? (min) : (x) > (max) ? (max) : (x))

WatchScalar::WatchScalar(QString name, PixelBox *f, QWidget *parent) 
    : WatchView(parent)
{
    /* Setup GUI */
    setupUi(this);
    setWindowTitle(name);

    QGridLayout *gridLayout;
    gridLayout = new QGridLayout(fContent);
    gridLayout->setSpacing(0);
    gridLayout->setMargin(0);

    m_qScrollArea = new QScrollArea();
    m_qScrollArea->setBackgroundRole(QPalette::Dark);
    gridLayout->addWidget(m_qScrollArea);
    
    m_pImageView = new ImageView();
    m_qScrollArea->setWidget(m_pImageView);
	
    m_pData = f;

    connect(m_pData, SIGNAL(dataChanged()), this, SLOT(updateData()));
    connect(m_pData, SIGNAL(dataDeleted()), this, SLOT(closeView()));
	connect(m_pImageView, SIGNAL(mousePosChanged(int, int)),
	        this, SLOT(setMousePos(int, int)));
	connect(m_pImageView, SIGNAL(picked(int, int)),
	        this, SLOT(newSelection(int, int)));
	connect(m_pImageView, SIGNAL(viewCenterChanged(int, int)),
	        this, SLOT(newViewCenter(int, int)));

    updateView(true);

	m_pImageView->setFocusPolicy(Qt::StrongFocus);
	m_pImageView->setFocus();	
}

void WatchScalar::updateView(bool covermapChanged)
{
    if (m_bNeedsUpdate || covermapChanged) {
        int x, y;
        bool  *pCover = m_pData->getCoveragePointer();
        float *pData  = m_pData->getDataPointer();
        QImage image = QImage(m_pData->getWidth(), m_pData->getHeight(), QImage::Format_RGB32);

        for (y=0; y<m_pData->getHeight(); y++) {
            for (x=0; x<m_pData->getWidth(); x++) {
                if (*pCover) {
                    int intensity;
                    if (*pData >= 0.0f) {
                        intensity = CLAMP((int)(*pData * 255), 0, 255);
                        image.setPixel(x, y, QColor(0,intensity,0).rgb());
                    } else {
                        intensity = CLAMP((int)(-(*pData) * 255), 0, 255);
                        image.setPixel(x, y, QColor(intensity,0,0).rgb());
                    }
                } else {
                    if ( ((x/8)%2) == ((y/8)%2)) {
                        image.setPixel(x, y, QColor(255,255,255).rgb());
                    } else {
                        image.setPixel(x, y, QColor(204,204,204).rgb());
                    }
                }
                pCover++;
                pData++;
            }
        }
		m_pImageView->resize(m_pData->getWidth(), m_pData->getHeight());
        m_pImageView->setImage(image);
    }
    m_bNeedsUpdate = false;
}

void WatchScalar::updateData()
{
    m_bNeedsUpdate = true;
}

void WatchScalar::closeView()
{
    hide();
	deleteLater();
}

void WatchScalar::setMousePos(int x, int y)
{
	if (x >= 0 && y >= 0) {
		bool active[3] = {true, false, false};
		float values[3];
		if (m_pData->getDataValue(x, y, &values[0])) {
			emit mouseOverValuesChanged(x, y, active, values);
		} else {
			emit mouseOverValuesChanged(-1, -1, active, values);
		}
	} else {
		emit mouseOverValuesChanged(-1, -1, NULL, NULL);
	}
}

void WatchScalar::newSelection(int x, int y)
{
	emit selectionChanged(x, y);
}

void WatchScalar::setZoomMode()
{
	m_pImageView->setMouseMode(ImageView::MM_ZOOM);
}

void WatchScalar::setPickMode()
{
	m_pImageView->setMouseMode(ImageView::MM_PICK);
}

void WatchScalar::newViewCenter(int x, int y)
{
	m_qScrollArea->ensureVisible(x, y, m_qScrollArea->width()/2 - 1,
	                             m_qScrollArea->height()/2 - 1);
}

void WatchScalar::setWorkspace(QWorkspace *ws)
{
	m_pImageView->setWorkspace(ws);
}

