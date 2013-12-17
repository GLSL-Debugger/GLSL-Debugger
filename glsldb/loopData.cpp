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

#include "loopData.qt.h"
#include "colors.qt.h"
#include <QtGui/QColor>

LoopData::LoopData(PixelBoxFloat *condition, QObject *parent) :
		QObject(parent)
{
	m_pActualVData = NULL;

	m_qModel.setHorizontalHeaderItem(0, new QStandardItem("iteration"));
	m_qModel.setHorizontalHeaderItem(1, new QStandardItem("active"));
	m_qModel.setHorizontalHeaderItem(2, new QStandardItem("done"));
	m_qModel.setHorizontalHeaderItem(3, new QStandardItem("out"));

	m_nIteration = 0;
	if (condition->getWidth() * condition->getHeight()) {
		m_pInitialCoverage = new bool[condition->getWidth()
				* condition->getHeight()];
		memcpy(m_pInitialCoverage, condition->getCoveragePointer(),
				condition->getWidth() * condition->getHeight() * sizeof(bool));
	}
	m_pActualFData = new PixelBoxFloat(condition);
	updateStatistic();
}

LoopData::LoopData(VertexBox *condition, QObject *parent) :
		QObject(parent)
{
	m_pActualFData = NULL;

	m_qModel.setHorizontalHeaderItem(0, new QStandardItem("iteration"));
	m_qModel.setHorizontalHeaderItem(1, new QStandardItem("active"));
	m_qModel.setHorizontalHeaderItem(2, new QStandardItem("done"));
	m_qModel.setHorizontalHeaderItem(3, new QStandardItem("out"));

	m_nIteration = 0;
	if (condition->getNumVertices()) {
		m_pInitialCoverage = new bool[condition->getNumVertices()];
		memcpy(m_pInitialCoverage, condition->getCoveragePointer(),
				condition->getNumVertices() * sizeof(bool));
	}
	m_pActualVData = new VertexBox();
	m_pActualVData->copyFrom(condition);
	updateStatistic();
}

LoopData::~LoopData()
{
	delete[] m_pInitialCoverage;
	delete m_pActualFData;
	delete m_pActualVData;
}

void LoopData::addLoopIteration(PixelBoxFloat *condition, int iteration)
{
	delete m_pActualFData;
	m_nIteration = iteration;
	m_pActualFData = new PixelBoxFloat(condition);
	updateStatistic();
}

void LoopData::addLoopIteration(VertexBox *condition, int iteration)
{
	m_nIteration = iteration;
	m_pActualVData->copyFrom(condition);
	updateStatistic();
}

void LoopData::updateStatistic(void)
{
	int x, y, c;
	int width;
	int height;
	int channel;
	float *pConditionData;
	bool *pConditionCover;
	bool *pInitialCover;

	if (m_pActualFData) {
		width = m_pActualFData->getWidth();
		height = m_pActualFData->getHeight();
		channel = m_pActualFData->getChannel();
		pConditionData = m_pActualFData->getDataPointer();
		pConditionCover = m_pActualFData->getCoveragePointer();
		pInitialCover = m_pInitialCoverage;
	} else if (m_pActualVData) {
		width = 1;
		height = m_pActualVData->getNumVertices();
		channel = 1;
		pConditionData = m_pActualVData->getDataPointer();
		pConditionCover = m_pActualVData->getCoveragePointer();
		pInitialCover = m_pInitialCoverage;
	} else {
		fprintf(stderr, "E! LoopData features vertex and fragment data\n");
		exit(1);
	}

	int nActive = 0;
	int nDone = 0;
	int nOut = 0;

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			if (*pInitialCover) {
				if (*pConditionCover) {
					if (*pConditionData > 0.75f) {
						nActive++;
					} else {
						nDone++;
					}
				} else {
					nOut++;
				}
			}
			for (c = 0; c < channel; c++) {
				pConditionData++;
			}
			pInitialCover++;
			pConditionCover++;
		}
	}

	m_nTotal = nActive + nDone + nOut;
	m_nActive = nActive;
	m_nDone = nDone;
	m_nOut = nOut;

	QList<QStandardItem*> rowData;
	rowData << new QStandardItem(QVariant(m_nIteration).toString());
	rowData << new QStandardItem(QVariant(nActive).toString());
	rowData << new QStandardItem(QVariant(nDone).toString());
	rowData << new QStandardItem(QVariant(nOut).toString());
	m_qModel.appendRow(rowData);
}

bool* LoopData::getActualCoverage(void)
{
	if (m_pActualFData) {
		return m_pActualFData->getCoveragePointer();
	}

	if (m_pActualVData) {
		return m_pActualVData->getCoveragePointer();
	}

	return NULL;
}

float* LoopData::getActualCondition(void)
{
	if (m_pActualFData) {
		return m_pActualFData->getDataPointer();
	}

	if (m_pActualVData) {
		return m_pActualVData->getDataPointer();
	}

	return NULL;
}

int LoopData::getWidth(void)
{
	if (m_pActualFData) {
		return m_pActualFData->getWidth();
	}

	if (m_pActualVData) {
		return 1;
	}

	return 0;
}

int LoopData::getHeight(void)
{
	if (m_pActualFData) {
		return m_pActualFData->getHeight();
	}
	if (m_pActualVData) {
		return m_pActualVData->getNumVertices();
	}

	return 0;
}

QImage LoopData::getImage(void)
{
	int x, y, c;
	int width = m_pActualFData->getWidth();
	int height = m_pActualFData->getHeight();
	int channel = m_pActualFData->getChannel();

	QImage image(width, height, QImage::Format_RGB32);

	float *pConditionData = m_pActualFData->getDataPointer();
	bool *pConditionCover = m_pActualFData->getCoveragePointer();
	bool *pInitialCover = m_pInitialCoverage;

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			if (*pInitialCover) {
				/* initial data available */
				if (*pConditionCover) {
					/* actual data available */
					if (*pConditionData > 0.75f) {
						image.setPixel(x, y, DBG_GREEN.rgb());
					} else {
						image.setPixel(x, y, DBG_RED.rgb());
					}
				} else {
					/* loop was left earlier at this pixel */
					image.setPixel(x, y, DBG_ORANGE.rgb());
				}
			} else {
				/* no initial data, print checkerboard */
				if (((x / 8) % 2) == ((y / 8) % 2)) {
					image.setPixel(x, y, QColor(255, 255, 255).rgb());
				} else {
					image.setPixel(x, y, QColor(204, 204, 204).rgb());
				}
			}
			for (c = 0; c < channel; c++) {
				pConditionData++;
			}
			pInitialCover++;
			pConditionCover++;
		}
	}
	return image;
}

