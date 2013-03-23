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

#ifndef _VERTEX_BOX_QT_H_
#define _VERTEX_BOX_QT_H_

#include <QtCore/QObject>
#include <QtCore/QVariant>

class VertexBox : public QObject
{
    Q_OBJECT

public:
    VertexBox(QObject *i_qParent = 0);
    VertexBox(float *i_pData , int numElementsPerVertex, int numVertices,
              int numPrimitives, bool *i_pCoverage, QObject *i_qParent = 0);
    ~VertexBox();

    void copyFrom(VertexBox* src);

    void setData(float *i_pData , int numElementsPerVertex, int numVertices,
	             int numPrimitives, bool *i_pCoverage = 0);
    
    void addVertexBox(VertexBox *f);
    
    void setNewCoverage(bool* i_pCoverage) { m_pCoverage = i_pCoverage; }
    
	bool* getCoverageFromData(bool *oldCoverage, bool *coverageChanged);
    bool* getCoveragePointer(void) { return m_pCoverage; }
    bool* getDataMapPointer(void) { return m_pDataMap; }
    float* getDataPointer(void) { return m_pData; }
	bool getDataValue(int numVertex, float *v);
	bool getDataValue(int numVertex, QVariant *v);
	int getNumElementsPerVertex(void) {return m_numElementsPerVertex; }
	int getNumVertices(void) { return m_numVertices; }
	int getNumPrimitives(void) { return m_numPrimitives; }
	
	void invalidateData();
	
	/* get min/max data values per element list, element == -1 means all
	 * elements of all vertices
	 */
	float getMin(int element = -1);
	float getMax(int element = -1);
	float getAbsMin(int element = -1);
	float getAbsMax(int element = -1);
	
signals:
    void dataChanged();
    void dataDeleted();

private:
    void calcMinMax();

    float *m_pData;
	bool *m_pDataMap;
	bool *m_pCoverage;
	int m_numElementsPerVertex;
	int m_numVertices;
	int m_numPrimitives;
    float *m_nMinData;
    float *m_nMaxData;
    float *m_nAbsMinData;
    float *m_nAbsMaxData;
};

#endif
