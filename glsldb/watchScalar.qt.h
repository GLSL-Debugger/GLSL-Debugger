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

#ifndef _WATCH_SCALAR_QT_H_
#define _WATCH_SCALAR_QT_H_

#include <QtGui/QScrollArea>

#include "ui/watchScalar.ui.h"
#include "pixelBox.qt.h"
#include "watchView.qt.h"
#include "imageView.qt.h"

class WatchScalar : public WatchView, public Ui::wWatchScalar {
    Q_OBJECT

public:
    WatchScalar(QString name, PixelBox *f, QWidget *parent = 0);
    void updateView(bool covermapChanged);
	void setWorkspace(QWorkspace *ws);
    void attachFpData(PixelBox *f, QString name) {};

public slots:
    void updateData();
    void closeView();
	void setZoomMode();
	void setPickMode();

signals:
	void mouseOverValuesChanged(int x, int y, const bool *active, const float *values);
	void selectionChanged(int x, int y);
	
private slots:	
	void setMousePos(int x, int y);
	void newSelection(int x, int y);
	void newViewCenter(int x, int y);
	
private:
    PixelBox    *m_pData;
    QScrollArea *m_qScrollArea;
    ImageView  *m_pImageView;
    bool         m_bNeedsUpdate;
};

#endif

