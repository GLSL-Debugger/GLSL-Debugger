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

#ifndef _LOOP_DATA_QT_H_
#define _LOOP_DATA_QT_H_

#include <QtGui/QStandardItemModel>
#include <QtGui/QStandardItem>
#include <QtCore/QObject>
#include <QtGui/QImage>
#include "pixelBox.qt.h"
#include "vertexBox.qt.h"

#define MAX_LOOP_ITERATIONS 255

class LoopData: public QObject {
Q_OBJECT

public:
	LoopData(PixelBoxFloat *condition, QObject *parent = 0);
	LoopData(VertexBox *condition, QObject *parent = 0);
	~LoopData();

	void addLoopIteration(PixelBoxFloat *condition, int iteration);
	void addLoopIteration(VertexBox *condition, int iteration);

	QStandardItemModel* getModel(void)
	{
		return &m_qModel;
	}

	bool* getInitialCoverage(void)
	{
		return m_pInitialCoverage;
	}
	bool* getActualCoverage(void);
	float* getActualCondition(void);

	PixelBoxFloat* getActualPixelBox(void)
	{
		return m_pActualFData;
	}
	VertexBox* getActualVertexBox(void)
	{
		return m_pActualVData;
	}

	int getTotal(void)
	{
		return m_nTotal;
	}
	int getActive(void)
	{
		return m_nActive;
	}
	int getDone(void)
	{
		return m_nDone;
	}
	int getOut(void)
	{
		return m_nOut;
	}

	int getWidth(void);
	int getHeight(void);
	int getIteration(void)
	{
		return m_nIteration;
	}

	QImage getImage(void);

	bool isFragmentLoop(void)
	{
		return (m_pActualFData != NULL);
	}
	bool isVertexLoop(void)
	{
		return (m_pActualVData != NULL);
	}

private slots:

private:
	void updateStatistic(void);

	int m_nIteration;
	bool *m_pInitialCoverage;
	PixelBoxFloat *m_pActualFData;
	VertexBox *m_pActualVData;

	int m_nTotal;
	int m_nActive;
	int m_nDone;
	int m_nOut;

	QStandardItemModel m_qModel;
};

#endif

