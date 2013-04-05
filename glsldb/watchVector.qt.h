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

#ifndef _WATCH_VECTOR_QT_H_
#define _WATCH_VECTOR_QT_H_

#include "ui_watchVector.h"
#include "pixelBox.qt.h"
#include "watchView.qt.h"
#include "imageView.qt.h"

#include <QtGui/QLabel>
#include <QtGui/QImage>
#include <QtGui/QWidget>
#include <QtGui/QScrollArea>

#define MAX_ATTACHMENTS 16

class WatchVector : public WatchView, public Ui::wWatchVector {
    Q_OBJECT

public:
    WatchVector(QWidget *parent = 0);
    void updateView(bool force);
    void attachFpData(PixelBox *f, QString name);
	void setWorkspace(QWorkspace *ws);

public slots:
    void on_cbRed_activated(int);
    void on_cbGreen_activated(int);
    void on_cbBlue_activated(int);
    void on_cbMapRed_activated(int);
    void on_cbMapGreen_activated(int);
    void on_cbMapBlue_activated(int);
	
	void on_tbMinRed_clicked();
	void on_tbMaxRed_clicked();
	void on_tbMinGreen_clicked();
	void on_tbMaxGreen_clicked();
	void on_tbMinBlue_clicked();
	void on_tbMaxBlue_clicked();

	void on_dsMinRed_valueChanged(double d);
	void on_dsMaxRed_valueChanged(double d);
	void on_dsMinGreen_valueChanged(double d);
	void on_dsMaxGreen_valueChanged(double d);
	void on_dsMinBlue_valueChanged(double d);
	void on_dsMaxBlue_valueChanged(double d);
	
	void on_tbSwitchRed_clicked();
	void on_tbSwitchGreen_clicked();
	void on_tbSwitchBlue_clicked();

    void on_tbSaveImage_clicked();
	
    void updateData();
    void detachData();
    void closeView();

	void setZoomMode();
	void setPickMode();
    void setMinMaxMode();
    void updateAllMinMax();

signals:
	void mouseOverValuesChanged(int x, int y, const bool *active, const QVariant *values);
	void selectionChanged(int x, int y);
	
private slots:	
	void setMousePos(int x, int y);
	void newSelection(int x, int y);
	void newViewCenter(int x, int y);
    void onMinMaxAreaChanged(void);

private:
    int  getNumFreeMappings(void);
    void updateGUI();
    void addMappingOptions(int idx);
    void delMappingOptions(int idx);
    int  getFirstFreeMapping(void);
    int  getIndexFromPixelBox(PixelBox *f);
	void getActiveChannels(PixelBox *channels[3]);
    QImage* drawNewImage(bool useAlpha);

    PixelBox    *m_pData[MAX_ATTACHMENTS];
    QString      m_qName[MAX_ATTACHMENTS];

    QScrollArea *m_qScrollArea;
    ImageView   *m_pImageView;
	int          m_viewCenter[2];
    bool         m_bNeedsUpdate;
    int          m_nActiveMappings;
};

#endif

