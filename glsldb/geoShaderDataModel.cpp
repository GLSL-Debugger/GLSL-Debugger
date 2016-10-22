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

//#include <QtGui>
#include "geoShaderDataModel.qt.h"

extern "C" {
#include "GL/gl.h"
#include "GL/glext.h"
#include "DebugLib/glenumerants.h"
#include "dbgprint.h"
}

#include "vertexBox.qt.h"
#include "geoShaderTreeItems.qt.h"

bool GeoShaderDataModel::isBasicPrimitive(int primType)
{
	switch (primType) {
	case GL_POINTS:
	case GL_LINES:
	case GL_LINE_STRIP:
	case GL_LINE_LOOP:
	case GL_LINES_ADJACENCY_EXT:
	case GL_LINE_STRIP_ADJACENCY_EXT:
	case GL_TRIANGLES:
	case GL_TRIANGLE_STRIP:
	case GL_TRIANGLE_FAN:
	case GL_TRIANGLE_MESH_SUN:
	case GL_TRIANGLES_ADJACENCY_EXT:
	case GL_TRIANGLE_STRIP_ADJACENCY_EXT:
		return true;
	case GL_QUADS:
	case GL_QUAD_STRIP:
	case GL_POLYGON:
	case GL_QUAD_MESH_SUN:
		return false;
	default:
		fprintf(stderr, "E! Unsupported primitive type\n");
		return false;
	}
}

GeoShaderDataModel::GeoShaderDataModel(int inPrimitiveType,
		int outPrimitiveType, VertexBox *primitiveMap,
		VertexBox *vertexCountMap, VertexBox *condition, bool *initialCoverage,
		QObject *parent) :
		QAbstractItemModel(parent)
{
	m_inPrimitiveType = inPrimitiveType;
	m_outPrimitiveType = outPrimitiveType;
	m_numSubInPrimitives = 0;
	m_numOutPrimitives = 0;
	m_numOutVertices = 0;
	m_numInPrimitives = 0;
	m_condition = condition;
	m_initialCoverage = initialCoverage;

	m_rootItem = new GeoShaderTreeItem(QString("ROOT"), -1, &m_currentData);

	float *vcMap = vertexCountMap->getDataPointer();
	float *primMap = primitiveMap->getDataPointer();

	float *pPrimMap = primMap;

	int j = 0, subPrimCount = 0;
	int totalVertices = primitiveMap->getNumVertices();
	int numInPrimitives = vertexCountMap->getNumVertices();

	for (int i = 0; i < numInPrimitives;) {
		int vertexCount, primitiveIdIn;
		primitiveIdIn = (int) vcMap[3 * i + 2];
		dbgPrint(DBGLVL_INFO, "INPUT PRIM %i\n", primitiveIdIn);
		GeoShaderTreeItem *inPrimItem;
		if (isBasicPrimitive(inPrimitiveType)) {
			inPrimItem = new GeoShaderTreeInPrimItem(
					QString("IN PRIM") + QString::number(primitiveIdIn), i,
					&m_currentData, condition, initialCoverage, m_rootItem);
		} else {
			inPrimItem = new GeoShaderTreeInPrimItem(
					QString("IN PRIM") + QString::number(primitiveIdIn), -1,
					NULL, condition, initialCoverage, m_rootItem);
		}
		m_numInPrimitives++;
		m_rootItem->appendChild(inPrimItem);
		GeoShaderTreeItem *currentInPrimItem = inPrimItem;
		do {
			vertexCount = (int) vcMap[3 * i];

			if (!isBasicPrimitive(inPrimitiveType)) {
				dbgPrint(DBGLVL_INFO,
						"\tSUB PRIM %i(%i)\n", primitiveIdIn, subPrimCount);
				GeoShaderTreeItem *inSubPrimItem = new GeoShaderTreeInPrimItem(
						QString("SUB PRIM") + QString::number(subPrimCount),
						subPrimCount, &m_currentData, condition,
						initialCoverage, inPrimItem);
				inPrimItem->appendChild(inSubPrimItem);
				currentInPrimItem = inSubPrimItem;
				subPrimCount++;
				m_numSubInPrimitives++;
			}

			if (vertexCount != 0) {
				int primitiveIdOut = (int) pPrimMap[0];
				int maxIndex = (int) pPrimMap[1] - 1;
				int numVertices = 0;
				int k = 0;
				dbgPrint(DBGLVL_INFO, "\t\tOUT PRIM %i\n", k);
				GeoShaderTreeItem *outPrimItem = NULL;
				if (!condition) {
					outPrimItem = new GeoShaderTreeOutPrimItem(
							QString("OUT PRIM ") + QString::number(k),
							currentInPrimItem);
					currentInPrimItem->appendChild(outPrimItem);
				}
				m_numOutPrimitives++;
				do {
					if (maxIndex < (int) pPrimMap[1]) {
						dbgPrint(DBGLVL_INFO, "\t\t\tVERTEX %i\n", numVertices);
						if (!condition) {
							GeoShaderTreeItem *vertexItem =
									new GeoShaderTreeVertexItem(
											QString("VERTEX ")
													+ QString::number(
															numVertices), j,
											&m_vertexData, outPrimItem);
							outPrimItem->appendChild(vertexItem);
						}
						m_numOutVertices++;
						numVertices++;
						maxIndex = (int) pPrimMap[1];
					}
					j++;
					pPrimMap += 3;
					if (numVertices < vertexCount
							&& (int) pPrimMap[0] != primitiveIdOut) {
						k++;
						primitiveIdOut = (int) pPrimMap[0];
						maxIndex = (int) pPrimMap[1] - 1;
						dbgPrint(DBGLVL_INFO, "\t\tOUT PRIM1 %i\n", k);
						if (!condition) {
							outPrimItem = new GeoShaderTreeOutPrimItem(
									QString("OUT PRIM ") + QString::number(k),
									currentInPrimItem);
							currentInPrimItem->appendChild(outPrimItem);
						}
						m_numOutPrimitives++;
					}
				} while (numVertices < vertexCount && j < totalVertices);
			}

			i++;
		} while (i < numInPrimitives && primitiveIdIn == (int) vcMap[3 * i + 2]);
	}
}

GeoShaderDataModel::~GeoShaderDataModel()
{
	m_currentData.clear();
	m_vertexData.clear();
	m_dataNames.clear();
	delete m_rootItem;
}

bool GeoShaderDataModel::addData(VertexBox *currentData, VertexBox* vertexData,
		QString &name)
{
	for (int i = 0; i < m_currentData.count(); i++) {
		if (m_currentData[i] == currentData) {
			return false;
		}
	}
	m_currentData.append(currentData);
	m_vertexData.append(vertexData);
	m_dataNames.append(name);
	connect(currentData, SIGNAL(dataDeleted()), this, SLOT(detachData()));
	connect(currentData, SIGNAL(dataChanged()), this, SLOT(updateData()));
	updateData();
	return true;
}

Qt::ItemFlags GeoShaderDataModel::flags(const QModelIndex &index) const
{
	if (!index.isValid()) {
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	}
	GeoShaderTreeItem *item =
			static_cast<GeoShaderTreeItem*>(index.internalPointer());
	return item->flags(index.column());
}

QModelIndex GeoShaderDataModel::index(int row, int column,
		const QModelIndex &parent) const
{
	GeoShaderTreeItem *parentItem;

	if (!parent.isValid()) {
		parentItem = m_rootItem;
	} else {
		parentItem = static_cast<GeoShaderTreeItem*>(parent.internalPointer());
	}

	GeoShaderTreeItem *childItem = parentItem->child(row);
	if (childItem) {
		return createIndex(row, column, childItem);
	} else {
		return QModelIndex();
	}
}

QModelIndex GeoShaderDataModel::parent(const QModelIndex &index) const
{
	if (!index.isValid()) {
		return QModelIndex();
	}

	GeoShaderTreeItem *childItem =
			static_cast<GeoShaderTreeItem*>(index.internalPointer());
	GeoShaderTreeItem *parentItem = childItem->parent();

	if (parentItem == m_rootItem) {
		return QModelIndex();
	}

	return createIndex(parentItem->row(), 0, parentItem);
}

int GeoShaderDataModel::rowCount(const QModelIndex &parent) const
{
	GeoShaderTreeItem *parentItem;

	if (!parent.isValid()) {
		parentItem = m_rootItem;
	} else {
		parentItem = static_cast<GeoShaderTreeItem*>(parent.internalPointer());
	}

	return parentItem->childCount();
}

int GeoShaderDataModel::columnCount(const QModelIndex &parent) const
{
	if (parent.isValid()) {
		return static_cast<GeoShaderTreeItem*>(parent.internalPointer())->columnCount();
	} else {
		if (m_condition) {
			return m_rootItem->columnCount() + 1;
		} else {
			return m_rootItem->columnCount();
		}
	}
}

QVariant GeoShaderDataModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid()) {
		return QVariant();
	} else if (role == Qt::DisplayRole) {
		GeoShaderTreeItem *item =
				static_cast<GeoShaderTreeItem*>(index.internalPointer());
		return item->data(index.column());
	} else if (role == Qt::TextColorRole) {
		GeoShaderTreeItem *item =
				static_cast<GeoShaderTreeItem*>(index.internalPointer());
		return item->displayColor(index.column());
	} else if (role == VertexRole) {
		GeoShaderTreeItem *item =
				static_cast<GeoShaderTreeItem*>(index.internalPointer());
		return item->isVertexItem();
	} else if (role == IndexRole) {
		GeoShaderTreeItem *item =
				static_cast<GeoShaderTreeItem*>(index.internalPointer());
		return item->getDataIndex();
	} else {
		return QVariant();
	}
}

bool GeoShaderDataModel::noOutputPrims(const QModelIndex &index) const
{
	if (index.isValid()) {
		GeoShaderTreeItem *item =
				static_cast<GeoShaderTreeItem*>(index.internalPointer());
		/* find input prim item */
		while (item->parent() != m_rootItem) {
			item = item->parent();
		}
		if (item->getDataIndex() == -1) {
			/* non-basic input primitive -> check children */
			for (int i = 0; i < item->childCount(); i++) {
				if (item->child(i)->childCount() != 0) {
					return false;
				}
			}
			return true;
		} else {
			return item->childCount() == 0;
		}
	}
	return false;
}

int GeoShaderDataModel::getBasePrimitive(int type)
{
	switch (type) {
	case GL_POINTS:
		return GL_POINTS;
	case GL_LINES:
	case GL_LINE_STRIP:
	case GL_LINE_LOOP:
		return GL_LINES;
	case GL_LINES_ADJACENCY_EXT:
	case GL_LINE_STRIP_ADJACENCY_EXT:
		return GL_LINES_ADJACENCY_EXT;
	case GL_TRIANGLES:
	case GL_QUADS:
	case GL_QUAD_STRIP:
	case GL_TRIANGLE_STRIP:
	case GL_TRIANGLE_FAN:
	case GL_POLYGON:
	case GL_QUAD_MESH_SUN:
	case GL_TRIANGLE_MESH_SUN:
		return GL_TRIANGLES;
	case GL_TRIANGLES_ADJACENCY_EXT:
	case GL_TRIANGLE_STRIP_ADJACENCY_EXT:
		return GL_TRIANGLES_ADJACENCY_EXT;
	default:
		return GL_INVALID_VALUE;
	}
}

QVariant GeoShaderDataModel::headerData(int section,
		Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole) {
		return QVariant();
	} else if (orientation == Qt::Horizontal) {
		if (section == 0) {
			return "Geometry Structure";
		} else {
			if (m_condition) {
				if (section == 1) {
					return QVariant("Condition");
				} else {
					return m_dataNames[section - 2];
				}
			} else {
				return m_dataNames[section - 1];
			}
		}
	} else {
		return QVariant();
	}
}

void GeoShaderDataModel::detachData(void)
{
	VertexBox *vb = static_cast<VertexBox*>(sender());

	if (!vb) {
		return;
	}

	int idx = m_currentData.indexOf(vb);
	if (idx >= 0) {
		m_currentData.removeAt(idx);
		m_vertexData.removeAt(idx);
		m_dataNames.removeAt(idx);
		updateData();
	}
	emit dataDeleted(idx);
	if (m_currentData.isEmpty()) {
		emit empty();
	}
}

void GeoShaderDataModel::updateData(void)
{
	QModelIndex indexBegin = index(0, 0);
	QModelIndex indexEnd = index(columnCount() - 1, rowCount() - 1);
	if (indexBegin.isValid() && indexEnd.isValid()) {
		emit dataChanged(indexBegin, indexEnd);
		emit layoutChanged();
	} else {
		beginResetModel();
		endResetModel();
	}
}

int GeoShaderDataModel::getNumInPrimitives() const
{
	return m_numInPrimitives;
}

int GeoShaderDataModel::getNumSubInPrimitives() const
{
	return m_numSubInPrimitives;
}

int GeoShaderDataModel::getNumOutPrimitives() const
{
	return m_numOutPrimitives;
}

int GeoShaderDataModel::getNumOutVertices() const
{
	return m_numOutVertices;
}

const QString& GeoShaderDataModel::getDataColumnName(int column) const
{
	static const QString es("");
	if (column >= 0 && column < m_dataNames.size()) {
		return m_dataNames[column];
	} else {
		return es;
	}
}

VertexBox* GeoShaderDataModel::getDataColumnVertexData(int column)
{
	if (column >= 0 && column < m_vertexData.size()) {
		return m_vertexData[column];
	} else {
		return NULL;
	}
}

VertexBox* GeoShaderDataModel::getDataColumnCurrentData(int column)
{
	if (column >= 0 && column < m_currentData.size()) {
		return m_currentData[column];
	} else {
		return NULL;
	}
}

int GeoShaderDataModel::getDataColumnCount() const
{
	return m_currentData.size();
}

GeoShaderDataSortFilterProxyModel::GeoShaderDataSortFilterProxyModel(
		QObject *parent) :
		QSortFilterProxyModel(parent)
{
	m_hideInactive = false;
	m_hideEmpty = false;
}

void GeoShaderDataSortFilterProxyModel::setHideInactive(bool b)
{
	m_hideInactive = b;
	beginResetModel();
	endResetModel();
}

void GeoShaderDataSortFilterProxyModel::setHideEmpty(bool b)
{
	m_hideEmpty = b;
	beginResetModel();
	endResetModel();
}

bool GeoShaderDataSortFilterProxyModel::filterAcceptsRow(int sourceRow,
		const QModelIndex &sourceParent) const
{
	bool accept = true;
	QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

	if (m_hideInactive && sourceModel()->flags(index) == 0) {
		accept = false;
	}

	if (m_hideEmpty
			&& static_cast<GeoShaderDataModel*>(sourceModel())->noOutputPrims(
					index)) {
		accept = false;
	}

	return accept;
}
