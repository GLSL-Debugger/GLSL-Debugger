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

#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <string.h>

#include <QtGui/QColor>
#include <QtGui/QMessageBox>
#include <QtCore/QVariant>

#include "dbgprint.h"

#define CLAMP(x, min, max) ((x) < (min) ? (min) : (x) > (max) ? (max) : (x))

template<typename vType>
TypedPixelBox<vType>::TypedPixelBox(int i_nWidth, int i_nHeight, int i_nChannel,
		vType *i_pData, bool *i_pCoverage, QObject *i_qParent) :
		PixelBox(i_qParent)
{
	int i;

	m_nWidth = i_nWidth;
	m_nHeight = i_nHeight;
	m_nChannel = i_nChannel;

	m_pData = new vType[m_nWidth * m_nHeight * m_nChannel];
	m_pDataMap = new bool[m_nWidth * m_nHeight];

	/* Initially use all given data */
	if (i_pData) {
		memcpy(m_pData, i_pData,
				m_nWidth * m_nHeight * m_nChannel * sizeof(vType));
	} else {
		memset(m_pData, 0, m_nWidth * m_nHeight * m_nChannel * sizeof(vType));
	}

	/* Initially use all given data */
	if (i_pCoverage) {
		memcpy(m_pDataMap, i_pCoverage, m_nWidth * m_nHeight * sizeof(bool));
	} else {
		for (i = 0; i < m_nWidth * m_nHeight; i++) {
			m_pDataMap[i] = true;
		}
	}
	m_pCoverage = i_pCoverage;

	if (i_nChannel && i_pData) {
		m_nMinData = new vType[m_nChannel];
		m_nMaxData = new vType[m_nChannel];
		m_nAbsMinData = new vType[m_nChannel];
		m_nAbsMaxData = new vType[m_nChannel];
		calcMinMax(this->m_minMaxArea);
	} else {
		m_nMinData = NULL;
		m_nMaxData = NULL;
		m_nAbsMinData = NULL;
		m_nAbsMaxData = NULL;
	}
}

template<typename vType>
TypedPixelBox<vType>::TypedPixelBox(TypedPixelBox *src) :
		PixelBox(src->parent())
{
	m_nWidth = src->m_nWidth;
	m_nHeight = src->m_nHeight;
	m_nChannel = src->m_nChannel;
	m_pCoverage = src->m_pCoverage;

	m_pData = new vType[m_nWidth * m_nHeight * m_nChannel];
	m_pDataMap = new bool[m_nWidth * m_nHeight];

	memcpy(m_pData, src->m_pData,
			m_nWidth * m_nHeight * m_nChannel * sizeof(vType));
	memcpy(m_pDataMap, src->m_pDataMap, m_nWidth * m_nHeight * sizeof(bool));

	m_nMinData = new vType[m_nChannel];
	m_nMaxData = new vType[m_nChannel];
	m_nAbsMinData = new vType[m_nChannel];
	m_nAbsMaxData = new vType[m_nChannel];

	memcpy(m_nMinData, src->m_nMinData, m_nChannel * sizeof(vType));
	memcpy(m_nMaxData, src->m_nMaxData, m_nChannel * sizeof(vType));
	memcpy(m_nAbsMinData, src->m_nAbsMinData, m_nChannel * sizeof(vType));
	memcpy(m_nAbsMaxData, src->m_nAbsMaxData, m_nChannel * sizeof(vType));
}

template<typename vType>
TypedPixelBox<vType>::~TypedPixelBox()
{
	delete[] m_pData;
	delete[] m_nMinData;
	delete[] m_nMaxData;
	delete[] m_nAbsMinData;
	delete[] m_nAbsMaxData;
}

template<typename vType>
void TypedPixelBox<vType>::calcMinMax(QRect area)
{
	int x, y, c;
	int left, right, top, bottom;
	bool *pCoverage;

	for (c = 0; c < m_nChannel; c++) {
		m_nMinData[c] = sc_maxVal;
		m_nMaxData[c] = sc_minVal;
		m_nAbsMinData[c] = sc_maxVal;
		m_nAbsMaxData[c] = (vType) 0;
	}

	if (m_pCoverage) {
		pCoverage = m_pCoverage;
	} else {
		pCoverage = m_pDataMap;
	}
	//pCoverage = m_pDataMap;

	if (area.isEmpty()) {
		left = top = 0;
		right = m_nWidth;
		bottom = m_nHeight;
	} else {
		left = (area.left() < 0) ? 0 : area.left();
		top = (area.top() < 0) ? 0 : area.top();
		// Note: area.right() != area.left() + area.width(), bottom same.
		right = (area.left() + area.width() > m_nWidth) ?
				m_nWidth : area.left() + area.width();
		bottom =
				(area.top() + area.height() > m_nHeight) ?
						m_nHeight : area.top() + area.height();
	}
	/*area.setCoords(0, 0, 2, 2);*/

	for (y = top; y < bottom; y++) {
		for (x = left; x < right; x++) {
			for (c = 0; c < m_nChannel; c++) {
				int idx = y * m_nWidth + x;
				if (pCoverage[idx]) {
					idx += c;
					if (m_pData[idx] < m_nMinData[c]) {
						m_nMinData[c] = m_pData[idx];
					}
					if (m_nMaxData[c] < m_pData[idx]) {
						m_nMaxData[c] = m_pData[idx];
					}
					if (fabs((double) m_pData[idx]) < m_nAbsMinData[c]) {
						m_nAbsMinData[c] = fabs((double) m_pData[idx]);
					}
					if (m_nAbsMaxData[c] < fabs((double) m_pData[idx])) {
						m_nAbsMaxData[c] = fabs((double) m_pData[idx]);
					}
				}
			}
		}
	}
}

template<typename vType>
double TypedPixelBox<vType>::getMin(int channel)
{
	if (channel == -1) {
		int c;
		vType min = sc_maxVal;
		calcMinMax(this->m_minMaxArea); /* FIXME: is it necessary? We already calculate min/max when the data changes */
		for (c = 0; c < m_nChannel; c++) {
			if (m_nMinData[c] < min) {
				min = m_nMinData[c];
			}
		}
		return (double) min;
	} else if (channel >= 0 && channel < m_nChannel) {
		return (double) m_nMinData[channel];
	} else {
		// TODO: Should this be silent?
		QString msg;
		msg.append(QString::number(channel));
		msg.append(" is not valid channel in TypedPixelBox::getMin.");
		msg.append("<BR>Please report this probem to "
				"<A HREF=\"mailto:glsldevil@vis.uni-stuttgart.de\">"
				"glsldevil@vis.uni-stuttgart.de</A>.");
		QMessageBox::critical(NULL, "Internal Error", msg, QMessageBox::Ok);
		return (double) 0;
	}
}

template<typename vType>
double TypedPixelBox<vType>::getMax(int channel)
{
	if (channel == -1) {
		int c;
		vType max = sc_minVal;
		calcMinMax(this->m_minMaxArea);
		for (c = 0; c < m_nChannel; c++) {
			if (m_nMaxData[c] > max) {
				max = m_nMaxData[c];
			}
		}
		return (double) max;
	} else if (channel >= 0 && channel < m_nChannel) {
		return (double) m_nMaxData[channel];
	} else {
		// TODO: Should this be silent?
		QString msg;
		msg.append(QString::number(channel));
		msg.append(" is not valid channel in TypedPixelBox::getMax.");
		msg.append("<BR>Please report this problem to "
				"<A HREF=\"mailto:glsldevil@vis.uni-stuttgart.de\">"
				"glsldevil@vis.uni-stuttgart.de</A>.");
		QMessageBox::critical(NULL, "Internal Error", msg, QMessageBox::Ok);
		return (double) 0;
	}
}

template<typename vType>
double TypedPixelBox<vType>::getAbsMin(int channel)
{
	if (channel == -1) {
		int c;
		vType min = sc_maxVal;
		calcMinMax(this->m_minMaxArea);
		for (c = 0; c < m_nChannel; c++) {
			if (m_nAbsMinData[c] < min) {
				min = m_nAbsMinData[c];
			}
		}
		return (double) min;
	} else if (channel >= 0 && channel < m_nChannel) {
		return (double) m_nAbsMinData[channel];
	} else {
		// TODO: Should this be silent?
		QString msg;
		msg.append(QString::number(channel));
		msg.append(" is not valid channel in TypedPixelBox::getAbsMin.");
		msg.append("<BR>Please report this probem to "
				"<A HREF=\"mailto:glsldevil@vis.uni-stuttgart.de\">"
				"glsldevil@vis.uni-stuttgart.de</A>.");
		QMessageBox::critical(NULL, "Internal Error", msg, QMessageBox::Ok);
		return (double) 0;
	}
}

template<typename vType>
double TypedPixelBox<vType>::getAbsMax(int channel)
{
	if (channel == -1) {
		int c;
		vType max = 0.0f;
		calcMinMax(this->m_minMaxArea);
		for (c = 0; c < m_nChannel; c++) {
			if (m_nAbsMaxData[c] > max) {
				max = m_nAbsMaxData[c];
			}
		}
		return (double) max;
	} else if (channel >= 0 && channel < m_nChannel) {
		return (double) m_nAbsMaxData[channel];
	} else {
		// TODO: Should this be silent?
		QString msg;
		msg.append(QString::number(channel));
		msg.append(" is not valid channel in TypedPixelBox::getAbsMax.");
		msg.append("<BR>Please report this probem to "
				"<A HREF=\"mailto:glsldevil@vis.uni-stuttgart.de\">"
				"glsldevil@vis.uni-stuttgart.de</A>.");
		QMessageBox::critical(NULL, "Internal Error", msg, QMessageBox::Ok);
		return (double) 0;
	}
}

template<typename vType>
void TypedPixelBox<vType>::setData(int i_nWidth, int i_nHeight, int i_nChannel,
		vType *i_pData, bool *i_pCoverage)
{
	int i;

	delete[] m_pData;
	delete[] m_pDataMap;
	delete[] m_nMinData;
	delete[] m_nMaxData;
	delete[] m_nAbsMinData;
	delete[] m_nAbsMaxData;

	m_nWidth = i_nWidth;
	m_nHeight = i_nHeight;
	m_nChannel = i_nChannel;

	m_pData = new vType[m_nWidth * m_nHeight * m_nChannel];
	m_pDataMap = new bool[m_nWidth * m_nHeight];

	/* Initially use all given data */
	if (i_pData) {
		memcpy(m_pData, i_pData,
				m_nWidth * m_nHeight * m_nChannel * sizeof(vType));
	} else {
		memset(m_pData, 0, m_nWidth * m_nHeight * m_nChannel * sizeof(vType));
	}

	/* Initially use all given data */
	if (i_pCoverage) {
		memcpy(m_pDataMap, i_pCoverage, m_nWidth * m_nHeight * sizeof(bool));
	} else {
		for (i = 0; i < m_nWidth * m_nHeight; i++) {
			m_pDataMap[i] = true;
		}
	}
	m_pCoverage = i_pCoverage;

	if (i_nChannel && i_pData) {
		m_nMinData = new vType[m_nChannel];
		m_nMaxData = new vType[m_nChannel];
		m_nAbsMinData = new vType[m_nChannel];
		m_nAbsMaxData = new vType[m_nChannel];
		calcMinMax(this->m_minMaxArea);
	} else {
		m_nMinData = NULL;
		m_nMaxData = NULL;
		m_nAbsMinData = NULL;
		m_nAbsMaxData = NULL;
	}
}

template<typename vType>
void TypedPixelBox<vType>::addPixelBox(TypedPixelBox<vType> *f)
{
	int i, j;
	vType *pDstData, *pSrcData;
	bool *pDstDataMap, *pSrcDataMap;

	if (m_nWidth != f->getWidth() || m_nHeight != f->getHeight()
			|| m_nChannel != f->getChannel()) {
		return;
	}

	pDstData = m_pData;
	pDstDataMap = m_pDataMap;

	pSrcData = f->getDataPointer();
	pSrcDataMap = f->getDataMapPointer();

	for (i = 0; i < m_nWidth * m_nHeight; i++) {
		if (*pSrcDataMap) {
			*pDstDataMap = true;
			for (j = 0; j < m_nChannel; j++) {
				*pDstData = *pSrcData;
				pDstData++;
				pSrcData++;
			}
		} else {
			for (j = 0; j < m_nChannel; j++) {
				pDstData++;
				pSrcData++;
			}
		}
		pDstDataMap++;
		pSrcDataMap++;
	}

	calcMinMax(this->m_minMaxArea);
	emit dataChanged();
}

template<typename vType>
bool* TypedPixelBox<vType>::getCoverageFromData(int *i_pActivePixels)
{
	int i, j;
	int nActivePixels;
	vType *pData;
	bool *pCoverage;
	bool *coverage = new bool[m_nWidth * m_nHeight];

	pCoverage = coverage;
	pData = m_pData;
	nActivePixels = 0;

	for (i = 0; i < m_nWidth * m_nHeight; i++) {
		if (*pData > 0.5f) {
			*pCoverage = true;
			nActivePixels++;
		} else {
			*pCoverage = false;
		}
		for (j = 0; j < m_nChannel; j++) {
			pData++;
		}
		pCoverage++;
	}

	if (i_pActivePixels) {
		*i_pActivePixels = nActivePixels;
	}

	return coverage;
}

template<typename vType>
int TypedPixelBox<vType>::mapFromValue(FBMapping i_eMapping, vType i_nF,
		int i_nC)
{
	switch (i_eMapping) {
	case FBM_MIN_MAX:
		return (int) (255 * (i_nF - m_nMinData[i_nC])
				/ (double) (m_nMaxData[i_nC] - m_nMinData[i_nC]));
	case FBM_CLAMP:
	default:
		return CLAMP((int)(i_nF * 255), 0, 255);
	}
}

template<typename vType>
QImage TypedPixelBox<vType>::getByteImage(FBMapping i_eMapping)
{
	int x, y, c;
	vType *pData;
	bool *pDataMap;

	QImage image(m_nWidth, m_nHeight, QImage::Format_RGB32);

	pData = m_pData;
	pDataMap = m_pDataMap;
	for (y = 0; y < m_nHeight; y++) {
		for (x = 0; x < m_nWidth; x++) {
			if (*pDataMap) {
				/* data available */
				QColor color;
				if (0 < m_nChannel) {
					color.setRed(mapFromValue(i_eMapping, *pData, 0));
					pData++;
				}
				if (1 < m_nChannel) {
					color.setGreen(mapFromValue(i_eMapping, *pData, 1));
					pData++;
				}
				if (2 < m_nChannel) {
					color.setBlue(mapFromValue(i_eMapping, *pData, 2));
					pData++;
				}
				for (c = 3; c < m_nChannel; c++) {
					pData++;
				}
				image.setPixel(x, y, color.rgb());
			} else {
				/* no data, print checkerboard */
				if (((x / 8) % 2) == ((y / 8) % 2)) {
					image.setPixel(x, y, QColor(255, 255, 255).rgb());
				} else {
					image.setPixel(x, y, QColor(204, 204, 204).rgb());
				}
				for (c = 0; c < m_nChannel; c++) {
					pData++;
				}
			}
			pDataMap++;
		}
	}
	return image;
}

template<typename vType>
void TypedPixelBox<vType>::setByteImageRedChannel(QImage *image,
		Mapping *mapping, RangeMapping *rangeMapping, float minmax[2],
		bool useAlpha)
{
	int x, y;

	if (m_nChannel != 1) {
		dbgPrint(DBGLVL_ERROR, "TypedPixelBox::setByteImageRedChannel(..)"
		" too many channels!");
		return;
	}

	if (!m_pCoverage || !m_pData) {
		dbgPrint(DBGLVL_ERROR, "TypedPixelBox::setByteImageRedChannel(..)"
		" no coverage or data!\n");
		return;
	}

	for (y = 0; y < m_nHeight; y++) {
		for (x = 0; x < m_nWidth; x++) {
			QColor c(image->pixel(x, y));
			unsigned long idx = y * m_nWidth + x;
			if (m_pCoverage[idx] && m_pDataMap[idx]) {
				c.setRed(
						getMappedValueI(m_pData[idx], mapping, rangeMapping,
								minmax));
			} else if (useAlpha) {
				c.setRed(0);
				c.setAlpha(0);
			} else {
				c.setRed(((x / 8) % 2) == ((y / 8) % 2) ? 255 : 204);
			}
			image->setPixel(x, y, c.rgb());
		}
	}
}

template<typename vType>
void TypedPixelBox<vType>::setByteImageGreenChannel(QImage *image,
		Mapping *mapping, RangeMapping *rangeMapping, float minmax[2],
		bool useAlpha)
{
	int x, y;

	if (m_nChannel != 1) {
		dbgPrint(DBGLVL_ERROR, "TypedPixelBox::setByteImageGreenChannel(..)"
		" too many channels!");
		return;
	}

	if (!m_pCoverage || !m_pData) {
		dbgPrint(DBGLVL_ERROR, "TypedPixelBox::setByteImageGreenChannel(..)"
		" no coverage or data!\n");
		return;
	}

	for (y = 0; y < m_nHeight; y++) {
		for (x = 0; x < m_nWidth; x++) {
			QColor c(image->pixel(x, y));
			unsigned long idx = y * m_nWidth + x;
			if (m_pCoverage[idx] && m_pDataMap[idx]) {
				c.setGreen(
						getMappedValueI(m_pData[idx], mapping, rangeMapping,
								minmax));
			} else if (useAlpha) {
				c.setGreen(0);
				c.setAlpha(0);
			} else {
				c.setGreen(((x / 8) % 2) == ((y / 8) % 2) ? 255 : 204);
			}
			image->setPixel(x, y, c.rgb());
		}
	}
}

template<typename vType>
void TypedPixelBox<vType>::setByteImageBlueChannel(QImage *image,
		Mapping *mapping, RangeMapping *rangeMapping, float minmax[2],
		bool useAlpha)
{
	int x, y;

	if (m_nChannel != 1) {
		dbgPrint(DBGLVL_ERROR, "TypedPixelBox::setByteImageBlueChannel(..)"
		" too many channels!");
		return;
	}

	if (!m_pCoverage || !m_pData) {
		dbgPrint(DBGLVL_ERROR, "TypedPixelBox::setByteImageBlueChannel(..)"
		" no coverage or data!\n");
		return;
	}

	for (y = 0; y < m_nHeight; y++) {
		for (x = 0; x < m_nWidth; x++) {
			QColor c(image->pixel(x, y));
			unsigned long idx = y * m_nWidth + x;
			if (m_pCoverage[idx] && m_pDataMap[idx]) {
				c.setBlue(
						getMappedValueI(m_pData[idx], mapping, rangeMapping,
								minmax));
			} else if (useAlpha) {
				c.setBlue(0);
				c.setAlpha(0);
			} else {
				c.setBlue(((x / 8) % 2) == ((y / 8) % 2) ? 255 : 204);
			}
			image->setPixel(x, y, c.rgb());
		}
	}
}

template<typename vType>
void TypedPixelBox<vType>::invalidateData()
{
	int x, y, c;
	bool *pDataMap;

	if (!m_pDataMap) {
		return;
	}

	pDataMap = m_pDataMap;
	for (y = 0; y < m_nHeight; y++) {
		for (x = 0; x < m_nWidth; x++) {
			*pDataMap++ = false;
		}
	}
	for (c = 0; c < m_nChannel; c++) {
		m_nMinData[c] = (vType) 0;
		m_nMaxData[c] = (vType) 0;
		m_nAbsMinData[c] = (vType) 0;
		m_nAbsMaxData[c] = (vType) 0;
	}
	emit dataChanged();
}

template<typename vType>
bool TypedPixelBox<vType>::getDataValue(int x, int y, vType *v)
{
	int i;
	if (m_pCoverage && m_pData && m_pCoverage[y * m_nWidth + x]
			&& m_pDataMap[y * m_nWidth + x]) {
		for (i = 0; i < m_nChannel; i++) {
			v[i] = m_pData[m_nChannel * (y * m_nWidth + x) + i];
		}
		return true;
	} else {
		return false;
	}
}

template<typename vType>
bool TypedPixelBox<vType>::getDataValue(int x, int y, QVariant *v)
{
	vType *value = new vType[m_nChannel];
	if (getDataValue(x, y, value)) {
		for (int i = 0; i < m_nChannel; i++) {
			v[i] = value[i];
		}
		delete[] value;
		return true;
	} else {
		delete[] value;
		return false;
	}
}

