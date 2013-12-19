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

#include "glCallStatistics.qt.h"
#include "textPercentDelegate.qt.h"

#include <QtCore/QAbstractItemModel>
#include <QtGui/QHeaderView>
#include <QtCore/QList>

GlCallStatistics::GlCallStatistics(QTableView *parent)
{
	m_pTableView = parent;

	m_pModel = new QStandardItemModel(m_pTableView);
	m_pProxyModel = new QSortFilterProxyModel(parent);
	m_pProxyModel->setSourceModel(m_pModel);
	m_pProxyModel->setDynamicSortFilter(true);

	m_pTableView->setModel(m_pProxyModel);

	m_pTableView->setSortingEnabled(true);
	m_pTableView->sortByColumn(0, Qt::DescendingOrder);

	resetStatistic();

	m_nNumCalls = 0;
}

GlCallStatistics::~GlCallStatistics()
{
}

void GlCallStatistics::resetStatistic(void)
{
	m_pModel->clear();
	m_pModel->setColumnCount(2);
	m_pModel->setRowCount(0);
	m_pModel->setHeaderData(0, Qt::Horizontal, "#");
	m_pModel->setHeaderData(1, Qt::Horizontal, "Function Call");

	m_pTableView->setColumnWidth(0, 50);
	m_pTableView->setColumnWidth(1, 250);

	TextPercentDelegate *delegate = new TextPercentDelegate(m_pTableView);
	m_pTableView->setItemDelegateForColumn(1, delegate);
	m_pTableView->verticalHeader()->hide();

	m_nNumCalls = 0;
}

void GlCallStatistics::incCallStatistic(QString i_qFName)
{
	m_nNumCalls++;

	/* Search for entry */
	QList<QStandardItem*> resultList;
	int row = 0;
	int i;

	resultList = m_pModel->findItems(i_qFName, Qt::MatchExactly, 1);

	if (resultList.count() == 0) {
		/* Add item to list */
		m_pModel->setRowCount(m_pModel->rowCount() + 1);
		row = m_pModel->rowCount() - 1;
		m_pModel->setData(m_pModel->index(row, 0),
				QVariant(Qt::AlignRight | Qt::AlignVCenter),
				Qt::TextAlignmentRole);
		m_pModel->setData(m_pModel->index(row, 0), 1);
		m_pModel->item(row, 0)->setEditable(false);
		m_pModel->setData(m_pModel->index(row, 1), i_qFName);
		m_pModel->item(row, 1)->setEditable(false);

		/* set row height of view */
		QModelIndex rowIndex = m_pModel->index(row, 0);
		if (rowIndex.isValid())
			m_pTableView->setRowHeight(m_pProxyModel->mapFromSource(rowIndex).row(), 22);
	} else if (resultList.count() == 1) {
		/* Exiting item */
		row = resultList[0]->row();
		int count = m_pModel->data(m_pModel->index(row, 0)).toInt();
		m_pModel->setData(m_pModel->index(row, 0), count + 1);
	} else {
		fprintf(stderr, "E! multiple entries for single call\n");
		exit(1);
	}

	/* Adapt percentages */
	for (i = 0; i < m_pModel->rowCount(); i++) {
		QModelIndex iCount = m_pModel->index(i, 0);
		int count = m_pModel->data(iCount).toInt();
		m_pModel->setData(m_pModel->index(i, 1), count / (float) m_nNumCalls,
				Qt::UserRole);
	}
}
