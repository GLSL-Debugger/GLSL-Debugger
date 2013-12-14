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

#include "pixelBox.qt.h"

#include "dbgprint.h"

PixelBox::PixelBox(QObject *i_qParent) :
		QObject(i_qParent)
{
	m_nWidth = 0;
	m_nHeight = 0;
	m_nChannel = 0;
	m_pDataMap = NULL;
	m_pCoverage = NULL;
}

PixelBox::~PixelBox()
{
	emit dataDeleted();

	delete[] m_pDataMap;
}

bool PixelBox::isAllDataAvailable()
{
	int x, y;
	bool *pDataMap;
	bool *pCoverage;

	if (!m_pDataMap) {
		return false;
	}

	pDataMap = m_pDataMap;
	pCoverage = m_pCoverage;

	for (y = 0; y < m_nHeight; y++) {
		for (x = 0; x < m_nWidth; x++) {
			if (*pCoverage && !*pDataMap) {
				dbgPrint(DBGLVL_INFO,
						"NOT ALL DATA AVILABLE, NEED READBACK =========================\n");
				return false;
			}
			pDataMap++;
			pCoverage++;
		}
	}
	return true;
}

void PixelBox::setMinMaxArea(const QRect& minMaxArea)
{
	this->m_minMaxArea = minMaxArea;
	emit minMaxAreaChanged();
}

template<> const float TypedPixelBox<float>::sc_minVal = -FLT_MAX;
template<> const float TypedPixelBox<float>::sc_maxVal = FLT_MAX;

template<> const int TypedPixelBox<int>::sc_minVal = INT_MIN;
template<> const int TypedPixelBox<int>::sc_maxVal = INT_MAX;

template<> const unsigned int TypedPixelBox<unsigned int>::sc_minVal = 0;
template<> const unsigned int TypedPixelBox<unsigned int>::sc_maxVal = UINT_MAX;

