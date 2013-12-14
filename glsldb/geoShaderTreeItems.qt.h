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

#ifndef GEOSHADERTREEITEMS_H
#define GEOSHADERTREEITEMS_H

#include <QtCore/QList>
#include <QtCore/QVariant>

class VertexBox;

class GeoShaderTreeItem {
public:
	GeoShaderTreeItem(QString name, int dataIdx, QList<VertexBox*> *data,
			GeoShaderTreeItem *parent = 0);
	virtual ~GeoShaderTreeItem();

	void appendChild(GeoShaderTreeItem *child);

	GeoShaderTreeItem *child(int row);
	int childCount() const;
	virtual int columnCount() const;
	virtual QVariant data(int column) const;
	virtual Qt::ItemFlags flags(int column) const;
	virtual QVariant displayColor(int column) const;
	int row() const;
	GeoShaderTreeItem *parent();

	int getDataIndex() const;
	virtual int isVertexItem() const;

protected:
	QList<GeoShaderTreeItem*> m_children;
	GeoShaderTreeItem *m_parentItem;
	QString m_name;
	int m_dataIdx;
	QList<VertexBox*> *m_data;
};

class GeoShaderTreeInPrimItem: public GeoShaderTreeItem {
public:
	GeoShaderTreeInPrimItem(QString name, int dataIdx, QList<VertexBox*> *data,
			VertexBox *condition, bool *initialCondition,
			GeoShaderTreeItem *parent = 0);

	virtual int columnCount() const;
	virtual QVariant data(int column) const;
	virtual QVariant displayColor(int column) const;
	virtual Qt::ItemFlags flags(int column) const;

protected:
	VertexBox *m_condition;
	bool *m_initialCondition;
};

class GeoShaderTreeOutPrimItem: public GeoShaderTreeItem {
public:
	GeoShaderTreeOutPrimItem(QString name, GeoShaderTreeItem *parent = 0);

	virtual QVariant data(int column) const;
	virtual Qt::ItemFlags flags(int column) const;
};

class GeoShaderTreeVertexItem: public GeoShaderTreeItem {
public:
	GeoShaderTreeVertexItem(QString name, int dataIdx, QList<VertexBox*> *data,
			GeoShaderTreeItem *parent = 0);

	virtual QVariant data(int column) const;
	virtual Qt::ItemFlags flags(int column) const;
	virtual int isVertexItem() const;
};
#endif

