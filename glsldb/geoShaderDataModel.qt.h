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

#ifndef GEOSHADERDATAMODEL_H
#define GEOSHADERDATAMODEL_H

#ifdef _WIN32
#include <windows.h>
#endif /* _WIN32 */

#include <QtCore/QAbstractItemModel>
#include <QtCore/QModelIndex>
#include <QtCore/QVariant>
#include <QtGui/QSortFilterProxyModel>

#include "geoShaderTreeItems.qt.h"

class VertexBox;

class GeoShaderDataSortFilterProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

 public:

	GeoShaderDataSortFilterProxyModel(QObject *parent = 0);

 public slots:

	void setHideInactive(bool b);
	void setHideEmpty(bool b);

 protected:

	virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

 private:

	bool m_hideInactive;
	bool m_hideEmpty;
};

class GeoShaderDataModel : public QAbstractItemModel
{
	Q_OBJECT

	public:
		GeoShaderDataModel(int inPrimitiveType, int outPrimitiveType,
		                   VertexBox *primitiveMap,
		                   VertexBox *vertexCount,
		                   VertexBox *condition,
                           bool      *initialCoverage,
						   QObject *parent = 0);
		~GeoShaderDataModel();

		QVariant data(const QModelIndex &index, int role) const;
		Qt::ItemFlags flags(const QModelIndex &index) const;
		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
		QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
		QModelIndex parent(const QModelIndex &index) const;
		int rowCount(const QModelIndex &parent = QModelIndex()) const;
		int columnCount(const QModelIndex &parent = QModelIndex()) const;

		bool noOutputPrims(const QModelIndex &index) const;

		bool addData(VertexBox *currentData, VertexBox* vertexData, QString &name);
		
		static bool isBasicPrimitive(int primType);
		static int getBasePrimitive(int type);

        int getNumInPrimitives() const;
        int getNumSubInPrimitives() const;
        int getNumOutPrimitives() const;
        int getNumOutVertices() const;
	
		int getDataColumnCount() const;
		const QString &getDataColumnName(int column) const;
		VertexBox* getDataColumnVertexData(int column);
		VertexBox* getDataColumnCurrentData(int column);
		
		enum DataRoles {VertexRole = Qt::UserRole, IndexRole = Qt::UserRole+1};

	signals:
		void empty();
		void dataDeleted(int i);

	private slots:
		void updateData();
		void detachData();
		
	private:
		int               m_inPrimitiveType;
		int               m_outPrimitiveType;
		int               m_numInPrimitives;
        int               m_numSubInPrimitives;
		int               m_numOutPrimitives;
		int               m_numOutVertices;
		VertexBox *       m_condition;
        bool *            m_initialCoverage;
		QList<VertexBox*> m_currentData;
		QList<VertexBox*> m_vertexData;
		QList<QString>    m_dataNames;

		GeoShaderTreeItem * m_rootItem;
};

#endif

