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

#ifndef _WATCH_TABLE_QT_H_
#define _WATCH_TABLE_QT_H_

#include "ui/watchTable.ui.h"
#include "vertexBox.qt.h"
#include "watchView.qt.h"
#include "mappings.h"

class VertexTableModel;
class VertexTableSortFilterProxyModel;
class GLScatter;

class WatchTable : public WatchView, public Ui::wWatchTable {
    Q_OBJECT

public:
    WatchTable(QWidget *parent = 0);
	~WatchTable();
    void updateView(bool force);
    void attachVpData(VertexBox *f, QString name);

public slots:
    void closeView();

signals:
	void selectionChanged(int vertId);
	
private slots:	
	void newSelection(const QModelIndex & index);

	void on_tbMinRed_clicked();
	void on_tbMinGreen_clicked();
	void on_tbMinBlue_clicked();
	void on_tbMinX_clicked();
	void on_tbMinY_clicked();
	void on_tbMinZ_clicked();
	
	void on_tbMaxRed_clicked();
	void on_tbMaxGreen_clicked();
	void on_tbMaxBlue_clicked();
	void on_tbMaxX_clicked();
	void on_tbMaxY_clicked();
	void on_tbMaxZ_clicked();
	
	void on_tbSwitchX_clicked();
	void on_tbSwitchY_clicked();
	void on_tbSwitchZ_clicked();
	void on_tbSwitchRed_clicked();
	void on_tbSwitchGreen_clicked();
	void on_tbSwitchBlue_clicked();
	
	void on_cbRed_activated(int newIdx);
	void on_cbGreen_activated(int newIdx);
	void on_cbBlue_activated(int newIdx);
	void on_cbX_activated(int newIdx);
	void on_cbY_activated(int newIdx);
	void on_cbZ_activated(int newIdx);
	
	void on_cbMapRed_activated(int newIdx);
	void on_cbMapGreen_activated(int newIdx);
	void on_cbMapBlue_activated(int newIdx);
	void on_cbMapX_activated(int newIdx);
	void on_cbMapY_activated(int newIdx);
	void on_cbMapZ_activated(int newIdx);
	
	void on_dsMinRed_valueChanged(double d);
	void on_dsMaxRed_valueChanged(double d);
	void on_dsMinGreen_valueChanged(double d);
	void on_dsMaxGreen_valueChanged(double d);
	void on_dsMinBlue_valueChanged(double d);
	void on_dsMaxBlue_valueChanged(double d);
	void on_dsMinX_valueChanged(double d);
	void on_dsMaxX_valueChanged(double d);
	void on_dsMinY_valueChanged(double d);
	void on_dsMaxY_valueChanged(double d);
	void on_dsMinZ_valueChanged(double d);
	void on_dsMaxZ_valueChanged(double d);
	
	void mappingDataChangedRed();
	void mappingDataChangedGreen();
	void mappingDataChangedBlue();
	void mappingDataChangedX();
	void mappingDataChangedY();
	void mappingDataChangedZ();
	
#if 0
	void mappingDataDetachedRed();
	void mappingDataDetachedGreen();
	void mappingDataDetachedBlue();
	void mappingDataDetachedX();
	void mappingDataDetachedY();
	void mappingDataDetachedZ();
#endif

	void detachData(int idx);

	void on_slPointSize_valueChanged(int value);

private:

	void updateGUI();
	void setupMappingUI();
	void addMappingOptions(int idx);
	void delMappingOptions(int idx);
	
	bool countsAllZero();
	void updateDataCurrent(float *data, int *count, int dataStride,
                    VertexBox *srcData, Mapping *mapping,
                    RangeMapping *rangeMapping,
                    float min, float max);

	VertexTableModel *m_vertexTable;
	VertexTableSortFilterProxyModel *m_filterProxy;

    GLScatter *m_qGLscatter;

	float *m_scatterPositions;
	float *m_scatterColorsAndSizes;
	int m_maxScatterDataElements;
	int m_scatterDataElements;

	float *m_scatterDataX;
	int m_scatterDataCountX;
	static const int m_scatterDataStrideX = 3;
	float *m_scatterDataY;
	int m_scatterDataCountY;
	static const int m_scatterDataStrideY = 3;
	float *m_scatterDataZ;
	int m_scatterDataCountZ;
	static const int m_scatterDataStrideZ = 3;
	float *m_scatterDataRed;
	int m_scatterDataCountRed;
	static const int m_scatterDataStrideRed = 3;
	float *m_scatterDataGreen;
	int m_scatterDataCountGreen;
	static const int m_scatterDataStrideGreen = 3;
	float *m_scatterDataBlue;
	int m_scatterDataCountBlue;
	static const int m_scatterDataStrideBlue = 3;
};

#endif

