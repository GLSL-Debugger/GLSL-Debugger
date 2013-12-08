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

#include "geoShaderTreeItems.qt.h"
#include "vertexBox.qt.h"
#include "colors.qt.h"

GeoShaderTreeItem::GeoShaderTreeItem(QString name, int dataIdx,
		QList<VertexBox*> *data, GeoShaderTreeItem *parent)
{
	m_parentItem = parent;
	m_name = name;
	m_dataIdx = dataIdx;
	m_data = data;
}

GeoShaderTreeItem::~GeoShaderTreeItem()
{
	qDeleteAll(m_children);
}

void GeoShaderTreeItem::appendChild(GeoShaderTreeItem *item)
{
	m_children.append(item);
}

GeoShaderTreeItem *GeoShaderTreeItem::child(int row)
{
	return m_children.value(row);
}

int GeoShaderTreeItem::childCount() const
{
	return m_children.count();
}

int GeoShaderTreeItem::columnCount() const
{
	if (m_data) {
		return m_data->count() + 1;
	} else {
		return m_parentItem->columnCount();
	}
}

GeoShaderTreeItem *GeoShaderTreeItem::parent()
{
	return m_parentItem;
}

int GeoShaderTreeItem::row() const
{
	if (m_parentItem) {
		return m_parentItem->m_children.indexOf(
				const_cast<GeoShaderTreeItem*>(this));
	}

	return 0;
}

int GeoShaderTreeItem::getDataIndex() const
{
	return m_dataIdx;
}

QVariant GeoShaderTreeItem::data(int) const
{
	return QVariant();
}

QVariant GeoShaderTreeItem::displayColor(int) const
{
	return QVariant();
}

Qt::ItemFlags GeoShaderTreeItem::flags(int) const
{
	return 0;
}

int GeoShaderTreeItem::isVertexItem() const
{
	return 0;
}

GeoShaderTreeInPrimItem::GeoShaderTreeInPrimItem(QString name, int dataIdx,
		QList<VertexBox*> *data, VertexBox *condition, bool *initialCondition,
		GeoShaderTreeItem *parent) :
		GeoShaderTreeItem(name, dataIdx, data, parent)
{
	m_condition = condition;
	m_initialCondition = initialCondition;
}

int GeoShaderTreeInPrimItem::columnCount() const
{
	if (m_data) {
		if (m_condition) {
			return m_data->count() + 2;
		} else {
			return m_data->count() + 1;
		}
	} else if (m_condition) {
		return 2;
	} else {
		return 1;
	}
}

QVariant GeoShaderTreeInPrimItem::data(int column) const
{
	if (m_condition) {
		if (column == 0) {
			return m_name;
		} else {
			if (m_initialCondition && column == 1) {
				if (m_initialCondition[m_dataIdx]) {
					if (m_condition->getCoveragePointer()
							&& m_condition->getCoveragePointer()[m_dataIdx]
									== true) {
						if (m_condition->getDataPointer()
								&& m_condition->getDataPointer()[m_dataIdx]
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
			} else if (flags(0) == 0) {
				return QVariant("%");
			} else if (column == 1) {
				if (m_condition->getDataPointer()[m_dataIdx] > 0.75f) {
					return QString("true");
				} else if (m_condition->getDataPointer()[m_dataIdx] > 0.25f) {
					return QString("false");
				} else {
					return QVariant("false");
				}
			} else if (m_data && column - 2 >= 0
					&& column - 2 < m_data->size()) {
				if (((*m_data)[column - 2]->getDataMapPointer()
						&& !(*m_data)[column - 2]->getDataMapPointer()[m_dataIdx])) {
					return QVariant("-");
				} else {
					return QVariant(
							(*m_data)[column - 2]->getDataPointer()[m_dataIdx]);
				}
			} else {
				return QVariant();
			}
		}
	} else {
		if (column == 0) {
			return m_name;
		} else if (m_data && column - 1 >= 0 && column - 1 < m_data->size()) {
			if (flags(0) == 0) {
				return QVariant("%");
			} else if (((*m_data)[column - 1]->getDataMapPointer()
					&& !(*m_data)[column - 1]->getDataMapPointer()[m_dataIdx])) {
				return QVariant("-");
			} else {
				return QVariant(
						(*m_data)[column - 1]->getDataPointer()[m_dataIdx]);
			}
		} else {
			return QVariant();
		}
	}
}

QVariant GeoShaderTreeInPrimItem::displayColor(int) const
{
	if (m_condition) {
		if (m_initialCondition) {
			if (m_initialCondition[m_dataIdx]) {
				if (m_condition->getCoveragePointer()
						&& m_condition->getCoveragePointer()[m_dataIdx]
								== true) {
					if (m_condition->getDataPointer()
							&& m_condition->getDataPointer()[m_dataIdx]
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
			if (m_condition->getDataPointer()[m_dataIdx] > 0.75f) {
				return DBG_GREEN;
			} else if (m_condition->getDataPointer()[m_dataIdx] > 0.25f) {
				return DBG_RED;
			} else {
				return QVariant();
			}
		}
	} else {
		return QVariant();
	}
}

Qt::ItemFlags GeoShaderTreeInPrimItem::flags(int column) const
{
	if (column == 0) {
		if (m_condition) {
			if (m_condition->getCoveragePointer()
					&& m_condition->getCoveragePointer()[m_dataIdx] == false) {
				return 0;
			} else if (!m_data) {
				/* non-basic input primitive */
				for (int i = 0; i < m_children.size(); i++) {
					Qt::ItemFlags cflags = m_children[i]->flags(column);
					if (cflags != 0) {
						return cflags;
					}
				}
				return 0;
			} else {
				return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
			}
		} else {
			if (m_data && !m_data->empty() && (*m_data)[0]
					&& ((*m_data)[0]->getCoveragePointer()
							&& !(*m_data)[0]->getCoveragePointer()[m_dataIdx])) {
				return 0;
			} else if (!m_data) {
				/* non-basic input primitive */
				for (int i = 0; i < m_children.size(); i++) {
					Qt::ItemFlags cflags = m_children[i]->flags(column);
					if (cflags != 0) {
						return cflags;
					}
				}
				return 0;
			} else {
				return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
			}
		}
	} else {
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	}
}

GeoShaderTreeOutPrimItem::GeoShaderTreeOutPrimItem(QString name,
		GeoShaderTreeItem *parent) :
		GeoShaderTreeItem(name, -1, NULL, parent)
{
}

QVariant GeoShaderTreeOutPrimItem::data(int column) const
{
	if (column == 0) {
		return m_name;
	} else {
		return QVariant();
	}
}

Qt::ItemFlags GeoShaderTreeOutPrimItem::flags(int column) const
{
	if (column == 0) {
		if (!m_data && m_parentItem) {
			return m_parentItem->flags(0);
		}
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	} else {
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	}
}

GeoShaderTreeVertexItem::GeoShaderTreeVertexItem(QString name, int dataIdx,
		QList<VertexBox*> *data, GeoShaderTreeItem *parent) :
		GeoShaderTreeItem(name, dataIdx, data, parent)
{
}

QVariant GeoShaderTreeVertexItem::data(int column) const
{
	if (column == 0) {
		return m_name;
	} else if (m_data && column - 1 >= 0 && column - 1 < m_data->size()) {
		float *dp = (*m_data)[column - 1]->getDataPointer();
		bool *dmp = (*m_data)[column - 1]->getDataMapPointer();
		if (!dmp[m_dataIdx]) {
			return QVariant("-");
		} else if (dp[2 * m_dataIdx + 1] == 0.0f
				|| dp[2 * m_dataIdx + 1] == -0.0f) {
			return QVariant(QString("?"));
		} else if (dp[2 * m_dataIdx + 1] < 0.0) {
			return QVariant("-");
		} else if (dp[2 * m_dataIdx + 1] > 0.0) {
			if (flags(0) == 0) {
				return QVariant("%");
			} else {
				return QVariant(dp[2 * m_dataIdx]);
			}
		} else {
			return QVariant();
		}
	} else {
		return QVariant();
	}
}

Qt::ItemFlags GeoShaderTreeVertexItem::flags(int column) const
{
	return m_parentItem->parent()->flags(column);
}

int GeoShaderTreeVertexItem::isVertexItem() const
{
	return 1;
}

