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

#include "colors.qt.h"
#include "vertexTableModel.qt.h"

VertexTableModel::VertexTableModel(QObject *parent) :
		QAbstractTableModel(parent)
{
	m_condition = NULL;
	m_pInitialCoverage = NULL;
}

VertexTableModel::~VertexTableModel()
{
	m_pData.clear();
	m_Names.clear();
}

bool VertexTableModel::addVertexBox(VertexBox *vb, QString &name)
{
	for (int i = 0; i < m_pData.count(); i++) {
		if (m_pData[i] == vb) {
			return false;
		}
	}
	m_pData.append(vb);
	m_Names.append(name);
	connect(vb, SIGNAL(dataDeleted()), this, SLOT(detachData()));
	connect(vb, SIGNAL(dataChanged()), this, SLOT(updateData()));
	beginResetModel();
	endResetModel();
	return true;
}

void VertexTableModel::setCondition(VertexBox *condition, bool *initialCoverage)
{
	m_condition = condition;
	m_pInitialCoverage = initialCoverage;
}

int VertexTableModel::rowCount(const QModelIndex &) const
{
	if (m_condition) {
		return m_condition->getNumVertices();
	} else if (m_pData.isEmpty()) {
		return 0;
	} else {
		return m_pData[0]->getNumVertices();
	}
}

int VertexTableModel::columnCount(const QModelIndex &) const
{
	if (m_condition) {
		return m_pData.size() + 1;
	} else {
		return m_pData.size();
	}
}

QVariant VertexTableModel::data(const QModelIndex &index, int role) const
{
	if (m_condition) {
		if (!index.isValid() || index.column() >= m_pData.size() + 1
				|| index.row() >= m_condition->getNumVertices()) {
			return QVariant();
		}

		switch (role) {
		case Qt::DisplayRole:
			if (m_pInitialCoverage && index.column() == 0) {
				if (m_pInitialCoverage[index.row()]) {
					if (m_condition->getCoveragePointer()[index.row()]
							== true) {
						if (m_condition->getDataPointer()[index.row()]
								> 0.75f) {
							return QString("active");
						} else {
							return QString("done");
						}
					} else {
						return QString("out");
					}
				} else {
					return QVariant("%");
				}
			} else if (m_condition->getCoveragePointer()[index.row()]) {
				if (index.column() == 0) {
					if (m_condition->getDataPointer()[index.row()] > 0.75f) {
						return QString("true");
					} else if (m_condition->getDataPointer()[index.row()]
							> 0.25f) {
						return QString("false");
					} else {
						return QVariant("false");
					}
				} else {
					if (!m_pData[index.column() - 1]->getDataMapPointer()[index.row()]) {
						return QString("-");
					} else {
						return m_pData[index.column() - 1]->getDataPointer()[index.row()];
					}
				}
			} else {
				return QString("%");
			}
		case Qt::TextColorRole:
			if (m_pInitialCoverage) {
				if (m_pInitialCoverage[index.row()]) {
					if (m_condition->getCoveragePointer()[index.row()]
							== true) {
						if (m_condition->getDataPointer()[index.row()]
								> 0.75f) {
							return DBG_GREEN;
						} else {
							return DBG_RED;
						}
					} else {
						return DBG_ORANGE;
					}
				} else {
					return QVariant();
				}
			} else {
				if (m_condition->getDataPointer()[index.row()] > 0.75f) {
					return DBG_GREEN;
				} else if (m_condition->getDataPointer()[index.row()] > 0.25f) {
					return DBG_RED;
				} else {
					return QVariant();
				}
			}
		default:
			return QVariant();
		}
	} else {
		if (!index.isValid() || index.column() >= m_pData.size()
				|| m_pData.empty()
				|| index.row() >= m_pData[0]->getNumVertices()) {
			return QVariant();
		}

		switch (role) {
		case Qt::DisplayRole:
			if (!m_pData[index.column()]->getCoveragePointer()[index.row()]) {
				return QString("%");
			} else if (!m_pData[index.column()]->getDataMapPointer()[index.row()]) {
				return QString("-");
			} else {
				return m_pData[index.column()]->getDataPointer()[index.row()];
			}
		default:
			return QVariant();
		}
	}
}

Qt::ItemFlags VertexTableModel::flags(const QModelIndex &index) const
{
	if (m_condition) {
		if (!index.isValid() || index.column() >= m_pData.size() + 1
				|| index.row() >= m_condition->getNumVertices()) {
			return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
		}
		if (m_condition->getCoveragePointer()[index.row()] == false) {
			return 0;
		} else {
			return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
		}
	} else {
		if (!index.isValid() || index.column() >= m_pData.size()
				|| m_pData.empty()
				|| index.row() >= m_pData[0]->getNumVertices()) {
			return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
		}

		if (!m_pData.empty()
				&& m_pData[0]->getCoveragePointer()[index.row()] == false) {
			return 0;
		} else {
			return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
		}
	}
}

QVariant VertexTableModel::headerData(int section, Qt::Orientation orientation,
		int role) const
{
	if (m_condition) {
		if (role != Qt::DisplayRole) {
			return QVariant();
		} else if (orientation == Qt::Horizontal) {
			if (section == 0) {
				return QString("Condition");
			} else {
				return m_Names[section - 1];
			}
		} else {
			return section;
		}
	} else {
		if (role != Qt::DisplayRole) {
			return QVariant();
		} else if (orientation == Qt::Horizontal) {
			return m_Names[section];
		} else {
			return section;
		}
	}
}

void VertexTableModel::detachData(void)
{
	VertexBox *vb = static_cast<VertexBox*>(sender());
	int idx = m_pData.indexOf(vb);
	if (idx >= 0) {
		m_pData.removeAt(idx);
		m_Names.removeAt(idx);
		beginResetModel();
		endResetModel();
	}
	emit dataDeleted(idx);
	if (m_pData.isEmpty()) {
		emit empty();
	}
}

void VertexTableModel::updateData(void)
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

VertexTableSortFilterProxyModel::VertexTableSortFilterProxyModel(
		QObject *parent) :
		QSortFilterProxyModel(parent)
{
	m_hideInactive = false;
}

void VertexTableSortFilterProxyModel::setHideInactive(bool b)
{
	m_hideInactive = b;
	/*
	 QModelIndex indexBegin = sourceModel()->index(0, 0);
	 QModelIndex indexEnd   = sourceModel()->index(sourceModel()->columnCount()-1,
	 sourceModel()->rowCount()-1);
	 emit dataChanged(indexBegin, indexEnd);
	 */
	beginResetModel();
	endResetModel();
}

bool VertexTableSortFilterProxyModel::filterAcceptsRow(int sourceRow,
		const QModelIndex &sourceParent) const
{
	QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
	if (m_hideInactive) {
		return sourceModel()->flags(index) != 0;
	} else {
		return true;
	}
}

const QString& VertexTableModel::getDataColumnName(int column) const
{
	static const QString es("");
	if (column >= 0 && column < m_Names.size()) {
		return m_Names[column];
	} else {
		return es;
	}
}

VertexBox* VertexTableModel::getDataColumn(int column)
{
	if (column >= 0 && column < m_pData.size()) {
		return m_pData[column];
	} else {
		return NULL;
	}
}

