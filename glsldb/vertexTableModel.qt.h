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

#ifndef VERTEX_TABLE_MODEL_H
#define VERTEX_TABLE_MODEL_H

#include <QtCore/QAbstractTableModel>
#include <QtCore/QSortFilterProxyModel>
#include "vertexBox.qt.h"

class VertexTableSortFilterProxyModel: public QSortFilterProxyModel {
Q_OBJECT

public:

	VertexTableSortFilterProxyModel(QObject *parent = 0);

public slots:

	void setHideInactive(bool b);

protected:

	virtual bool filterAcceptsRow(int sourceRow,
			const QModelIndex &sourceParent) const;

private:

	bool m_hideInactive;
};

class VertexTableModel: public QAbstractTableModel {
Q_OBJECT

public:

	VertexTableModel(QObject *parent = 0);
	~VertexTableModel();

	bool addVertexBox(VertexBox *vb, QString &name);

	void setCondition(VertexBox *condition, bool *initialCoverage = NULL);

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role =
			Qt::DisplayRole) const;
	const QString &getDataColumnName(int column) const;
	VertexBox* getDataColumn(int column);

public slots:
	void updateData();

signals:
	void empty();
	void dataDeleted(int idx);

private slots:

	void detachData();

private:

	VertexTableSortFilterProxyModel *m_filterProxy;

	QList<VertexBox*> m_pData;
	QList<QString> m_Names;
	VertexBox *m_condition;
	bool *m_pInitialCoverage;
};

#endif

