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

#ifndef _FLOAT_BOX_QT_H_
#define _FLOAT_BOX_QT_H_

#ifndef USE_MESA
    #include "ShaderLang.h"
#endif

#include <QtCore/QObject>
#include <QtGui/QImage>

#include "mappings.h"

class PixelBox: public QObject {
Q_OBJECT

public:
	PixelBox(QObject *i_qParent = 0);
	virtual ~PixelBox();

	void setNewCoverage(bool* i_pCoverage)
	{
		m_pCoverage = i_pCoverage;
	}

	virtual bool* getCoverageFromData(int *i_pActivePixels = NULL) = 0;
	bool* getCoveragePointer(void)
	{
		return m_pCoverage;
	}
	bool* getDataMapPointer(void)
	{
		return m_pDataMap;
	}

	int getWidth(void)
	{
		return m_nWidth;
	}
	int getHeight(void)
	{
		return m_nHeight;
	}
	int getChannel(void)
	{
		return m_nChannel;
	}

	/* get min/max data values per channel, channel == -1 means all channels */
	virtual double getMin(int channel = -1) = 0;
	virtual double getMax(int channel = -1) = 0;
	virtual double getAbsMin(int channel = -1) = 0;
	virtual double getAbsMax(int channel = -1) = 0;

	enum FBMapping {
		FBM_CLAMP,
		FBM_MIN_MAX
	};
	virtual QImage getByteImage(FBMapping i_eMapping) = 0;
	virtual void setByteImageRedChannel(QImage *image, Mapping *mapping,
			RangeMapping *rangeMapping, float minmax[2], bool useAlpha) = 0;
	virtual void setByteImageGreenChannel(QImage *image, Mapping *mapping,
			RangeMapping *rangeMapping, float minmax[2], bool useAlpha) = 0;
	virtual void setByteImageBlueChannel(QImage *image, Mapping *mapping,
			RangeMapping *rangeMapping, float minmax[2], bool useAlpha) = 0;

	virtual bool getDataValue(int x, int y, QVariant *v) = 0;

	bool isAllDataAvailable();
	virtual void invalidateData() = 0;

signals:
	void dataChanged();
	void dataDeleted();
	void minMaxAreaChanged();

public slots:
	void setMinMaxArea(const QRect& minMaxArea);

protected:
	int m_nWidth;
	int m_nHeight;
	int m_nChannel;
	bool *m_pDataMap;
	bool *m_pCoverage;
	QRect m_minMaxArea;
};

template<typename vType> class TypedPixelBox: public PixelBox {
public:
	TypedPixelBox(int i_nWidth, int i_nHeight, int i_nChannel, vType *i_pData,
			bool *i_pCoverage = 0, QObject *i_qParent = 0);
	TypedPixelBox(TypedPixelBox *src);
	virtual ~TypedPixelBox();

	void setData(int i_nWidth, int i_nHeight, int i_nChannel, vType *i_pData,
			bool *i_pCoverage = 0);
	void addPixelBox(TypedPixelBox *f);

	virtual bool* getCoverageFromData(int *i_pActivePixels = NULL);
	vType* getDataPointer(void)
	{
		return m_pData;
	}
	bool getDataValue(int x, int y, vType *v);
	virtual bool getDataValue(int x, int y, QVariant *v);

	/* get min/max data values per channel, channel == -1 means all channels */
	virtual double getMin(int channel = -1);
	virtual double getMax(int channel = -1);
	virtual double getAbsMin(int channel = -1);
	virtual double getAbsMax(int channel = -1);

	virtual QImage getByteImage(FBMapping i_eMapping);

	virtual void setByteImageRedChannel(QImage *image, Mapping *mapping,
			RangeMapping *rangeMapping, float minmax[2], bool useAlpha);
	virtual void setByteImageGreenChannel(QImage *image, Mapping *mapping,
			RangeMapping *rangeMapping, float minmax[2], bool useAlpha);
	virtual void setByteImageBlueChannel(QImage *image, Mapping *mapping,
			RangeMapping *rangeMapping, float minmax[2], bool useAlpha);

	virtual void invalidateData();

protected:

	static const vType sc_minVal;
	static const vType sc_maxVal;

	void calcMinMax(QRect area);
	int mapFromValue(FBMapping i_eMapping, vType i_nF, int i_nC);

	vType *m_pData;
	vType *m_nMinData;
	vType *m_nMaxData;
	vType *m_nAbsMinData;
	vType *m_nAbsMaxData;
};

typedef TypedPixelBox<float> PixelBoxFloat;
typedef TypedPixelBox<int> PixelBoxInt;
typedef TypedPixelBox<unsigned int> PixelBoxUInt;

// include template definitions
#include "pixelBox.inl.h"

#endif

