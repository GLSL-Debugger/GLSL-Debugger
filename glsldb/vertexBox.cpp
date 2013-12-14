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

#include <math.h>
#include <float.h>

#include "vertexBox.qt.h"
#include "dbgprint.h"

VertexBox::VertexBox(QObject *i_qParent) :
		QObject(i_qParent)
{
	m_pData = NULL;
	m_numElementsPerVertex = 0;
	m_numVertices = 0;
	m_numPrimitives = 0;
	m_pDataMap = NULL;
	m_pCoverage = NULL;
	m_nMinData = NULL;
	m_nMaxData = NULL;
	m_nAbsMinData = NULL;
	m_nAbsMaxData = NULL;
}

VertexBox::VertexBox(float *i_pData, int i_numElementsPerVertex,
		int i_numVertices, int i_numPrimitives, bool *i_pCoverage,
		QObject *i_qParent)
{
	UNUSED_ARG(i_qParent)
	setData(i_pData, i_numElementsPerVertex, i_numVertices, i_numPrimitives,
			i_pCoverage);
}

VertexBox::~VertexBox()
{
	emit dataDeleted();
	delete[] m_pData;
	delete[] m_pDataMap;
	delete[] m_nMinData;
	delete[] m_nMaxData;
	delete[] m_nAbsMinData;
	delete[] m_nAbsMaxData;
}

void VertexBox::copyFrom(VertexBox *src)
{
	delete[] m_pData;
	delete[] m_pDataMap;
	delete[] m_nMinData;
	delete[] m_nMaxData;
	delete[] m_nAbsMinData;
	delete[] m_nAbsMaxData;

	m_numElementsPerVertex = src->m_numElementsPerVertex;
	m_numVertices = src->m_numVertices;
	m_numPrimitives = src->m_numPrimitives;

	m_pData = new float[m_numVertices * m_numElementsPerVertex];
	m_pDataMap = new bool[m_numVertices];

	memcpy(m_pData, src->m_pData,
			m_numVertices * m_numElementsPerVertex * sizeof(float));
	memcpy(m_pDataMap, src->m_pDataMap, m_numVertices * sizeof(bool));

	m_pCoverage = src->m_pCoverage;

	m_nMinData = new float[m_numElementsPerVertex];
	m_nMaxData = new float[m_numElementsPerVertex];
	m_nAbsMinData = new float[m_numElementsPerVertex];
	m_nAbsMaxData = new float[m_numElementsPerVertex];

	memcpy(m_nMinData, src->m_nMinData, m_numElementsPerVertex * sizeof(float));
	memcpy(m_nMaxData, src->m_nMaxData, m_numElementsPerVertex * sizeof(float));
	memcpy(m_nAbsMinData, src->m_nAbsMinData,
			m_numElementsPerVertex * sizeof(float));
	memcpy(m_nAbsMaxData, src->m_nAbsMaxData,
			m_numElementsPerVertex * sizeof(float));

}

void VertexBox::setData(float *i_pData, int i_numElementsPerVertex,
		int i_numVertices, int i_numPrimitives, bool *i_pCoverage)
{
	int i;

	if (m_numVertices > 0) {
		delete[] m_pData;
		delete[] m_pDataMap;
		delete[] m_nMinData;
		delete[] m_nMaxData;
		delete[] m_nAbsMinData;
		delete[] m_nAbsMaxData;
		m_pData = NULL;
		m_pDataMap = NULL;
	}
	if (i_numVertices > 0 && i_pData) {
		m_pData = new float[i_numVertices * i_numElementsPerVertex];
		m_pDataMap = new bool[i_numVertices];
		memcpy(m_pData, i_pData,
				i_numVertices * i_numElementsPerVertex * sizeof(float));
		m_numElementsPerVertex = i_numElementsPerVertex;
		m_numVertices = i_numVertices;
		m_numPrimitives = i_numPrimitives;
		/* Initially use all given data */
		if (i_pCoverage) {
			memcpy(m_pDataMap, i_pCoverage, m_numVertices * sizeof(bool));
		} else {
			for (i = 0; i < m_numVertices; i++) {
				m_pDataMap[i] = true;
			}
		}
		m_pCoverage = i_pCoverage;

		m_nMinData = new float[m_numElementsPerVertex];
		m_nMaxData = new float[m_numElementsPerVertex];
		m_nAbsMinData = new float[m_numElementsPerVertex];
		m_nAbsMaxData = new float[m_numElementsPerVertex];
		calcMinMax();
	} else {
		m_pData = NULL;
		m_pDataMap = NULL;
		m_pCoverage = NULL;
		m_numElementsPerVertex = 0;
		m_numVertices = 0;
		m_numPrimitives = 0;
		m_nMinData = NULL;
		m_nMaxData = NULL;
		m_nAbsMinData = NULL;
		m_nAbsMaxData = NULL;
	}

	dbgPrint(DBGLVL_INFO,
			"VertexBox::setData: numPrimitives=%i numVertices=%i numElementsPerVertex=%i\n", m_numPrimitives, m_numVertices, m_numElementsPerVertex);

	emit dataChanged();
}

bool* VertexBox::getCoverageFromData(bool *oldCoverage, bool *coverageChanged)
{
	int i;
	float *pData;
	bool *pCoverage, *pOldCoverage;
	bool *coverage = new bool[m_numVertices];

	if (oldCoverage) {
		*coverageChanged = false;
	} else {
		*coverageChanged = true;
	}

	pCoverage = coverage;
	pOldCoverage = oldCoverage;
	pData = m_pData;

	for (i = 0; i < m_numVertices; i++) {
		if (*pData > 0.5f) {
			*pCoverage = true;
		} else {
			*pCoverage = false;
		}
		if (oldCoverage && *pCoverage != *pOldCoverage) {
			*coverageChanged = true;
		}
		pOldCoverage++;
		pCoverage++;
		pData += m_numElementsPerVertex;
	}
	return coverage;
}

void VertexBox::addVertexBox(VertexBox *f)
{
	int i, j;
	float *pDstData, *pSrcData;
	bool *pDstDataMap, *pSrcDataMap;

	if (m_numVertices != f->getNumVertices()
			|| m_numPrimitives != f->getNumPrimitives()
			|| m_numElementsPerVertex != f->getNumElementsPerVertex()) {
		return;
	}

	pDstData = m_pData;
	pDstDataMap = m_pDataMap;

	pSrcData = f->getDataPointer();
	pSrcDataMap = f->getDataMapPointer();

	for (i = 0; i < m_numVertices; i++) {
		for (j = 0; j < m_numElementsPerVertex; j++) {
			if (*pSrcDataMap) {
				*pDstDataMap = true;
				*pDstData = *pSrcData;
				pDstData++;
				pSrcData++;
			} else {
				pDstData++;
				pSrcData++;
			}
		}
		pDstDataMap++;
		pSrcDataMap++;
	}
	calcMinMax();

	emit dataChanged();
}

void VertexBox::calcMinMax()
{
	int v, c;
	float *pData;
	bool *pCoverage;

	for (c = 0; c < m_numElementsPerVertex; c++) {
		m_nMinData[c] = FLT_MAX;
		m_nMaxData[c] = -FLT_MAX;
		m_nAbsMinData[c] = FLT_MAX;
		m_nAbsMaxData[c] = 0.0f;
	}

	pData = m_pData;
	if (m_pCoverage) {
		pCoverage = m_pCoverage;
	} else {
		pCoverage = m_pDataMap;
	}
	//pCoverage = m_pDataMap;

	for (v = 0; v < m_numVertices; v++) {
		for (c = 0; c < m_numElementsPerVertex; c++) {
			if (*pCoverage) {
				if (*pData < m_nMinData[c]) {
					m_nMinData[c] = *pData;
				}
				if (m_nMaxData[c] < *pData) {
					m_nMaxData[c] = *pData;
				}
				if (fabs(*pData) < m_nAbsMinData[c]) {
					m_nAbsMinData[c] = fabs(*pData);
				}
				if (m_nAbsMaxData[c] < fabs(*pData)) {
					m_nAbsMaxData[c] = fabs(*pData);
				}
			}
			pData++;
		}
		pCoverage++;
	}
}

float VertexBox::getMin(int element)
{
	if (element == -1) {
		int c;
		float min = FLT_MAX;
		for (c = 0; c < m_numElementsPerVertex; c++) {
			if (m_nMinData[c] < min) {
				min = m_nMinData[c];
			}
		}
		return min;
	} else if (element >= 0 && element < m_numElementsPerVertex) {
		return m_nMinData[element];
	} else {
		dbgPrint(DBGLVL_WARNING,
				"VertexBox::getMin: invalid element param %i\n", element);
		return 0.0f;
	}
}

float VertexBox::getMax(int element)
{
	if (element == -1) {
		int c;
		float max = -FLT_MAX;
		for (c = 0; c < m_numElementsPerVertex; c++) {
			if (m_nMaxData[c] > max) {
				max = m_nMaxData[c];
			}
		}
		return max;
	} else if (element >= 0 && element < m_numElementsPerVertex) {
		return m_nMaxData[element];
	} else {
		dbgPrint(DBGLVL_WARNING,
				"VertexBox::getMax: invalid element param %i\n", element);
		return 0.0f;
	}
}

float VertexBox::getAbsMin(int element)
{
	if (element == -1) {
		int c;
		float min = FLT_MAX;
		for (c = 0; c < m_numElementsPerVertex; c++) {
			if (m_nAbsMinData[c] < min) {
				min = m_nAbsMinData[c];
			}
		}
		return min;
	} else if (element >= 0 && element < m_numElementsPerVertex) {
		return m_nAbsMinData[element];
	} else {
		dbgPrint(DBGLVL_WARNING,
				"VertexBox::getAbsMin: invalid element param %i\n", element);
		return 0.0f;
	}
}

float VertexBox::getAbsMax(int element)
{
	if (element == -1) {
		int c;
		float max = 0.0f;
		for (c = 0; c < m_numElementsPerVertex; c++) {
			if (m_nAbsMaxData[c] > max) {
				max = m_nAbsMaxData[c];
			}
		}
		return max;
	} else if (element >= 0 && element < m_numElementsPerVertex) {
		return m_nAbsMaxData[element];
	} else {
		dbgPrint(DBGLVL_WARNING,
				"VertexBox::getAbsMax: invalid element param %i\n", element);
		return 0.0f;
	}
}

void VertexBox::invalidateData()
{
	int i, c;
	bool *pDataMap;

	if (!m_pDataMap) {
		return;
	}
	dbgPrint(DBGLVL_DEBUG, "VertexBox::invalidateData()\n");
	pDataMap = m_pDataMap;
	for (i = 0; i < m_numVertices; i++) {
		*pDataMap++ = false;
	}
	for (c = 0; c < m_numElementsPerVertex; c++) {
		m_nMinData[c] = 0.f;
		m_nMaxData[c] = 0.f;
		m_nAbsMinData[c] = 0.f;
		m_nAbsMaxData[c] = 0.f;
	}
	emit dataChanged();
}

bool VertexBox::getDataValue(int numVertex, float *v)
{
	int i;
	bool *pCoverage;

	if (m_pCoverage) {
		pCoverage = m_pCoverage;
	} else {
		pCoverage = m_pDataMap;
	}
	if (pCoverage && m_pData && pCoverage[numVertex * m_numElementsPerVertex]) {
		for (i = 0; i < m_numElementsPerVertex; i++) {
			v[i] = m_pData[m_numElementsPerVertex * numVertex + i];
		}
		return true;
	} else {
		return false;
	}
}

bool VertexBox::getDataValue(int numVertex, QVariant *v)
{
	int i;
	bool *pCoverage;

	if (m_pCoverage) {
		pCoverage = m_pCoverage;
	} else {
		pCoverage = m_pDataMap;
	}
	if (pCoverage && m_pData && pCoverage[numVertex * m_numElementsPerVertex]) {
		for (i = 0; i < m_numElementsPerVertex; i++) {
			v[i] = m_pData[m_numElementsPerVertex * numVertex + i];
		}
		return true;
	} else {
		return false;
	}
}
