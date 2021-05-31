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

#include "glTraceSettingsDialog.qt.h"
#include <QtWidgets/QAbstractItemDelegate>
#include <QtWidgets/QItemDelegate>
#include <QtWidgets/QItemEditorFactory>
#include <QtWidgets/QPushButton>

#ifdef _WIN32
#include <wingdi.h>
#else

#endif

GlTraceSettingsViewFilter::GlTraceSettingsViewFilter(QObject *parent) :
		QSortFilterProxyModel(parent)
{
	filterPattern = QString();
}

void GlTraceSettingsViewFilter::setFilterWildcard(const QString &pattern)
{
	filterPattern = pattern;
	invalidateFilter();
}

bool GlTraceSettingsViewFilter::filterAcceptsRow(int sourceRow,
		const QModelIndex &sourceParent) const
{
	if (sourceModel()) {
		QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
		return ((GlTraceFilterModel*) sourceModel())->itemContainsPatternRecursive(
				index, filterPattern);
	} else {
		return true;
	}
}

GlTraceSettingsDialog::GlTraceSettingsDialog(GlTraceFilterModel *model,
		QWidget *parent) :
		QDialog(parent)
{
	setupUi(this);

	treeView->setEditTriggers(
			QAbstractItemView::CurrentChanged
					| QAbstractItemView::SelectedClicked);
	treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
	treeView->setSelectionMode(QAbstractItemView::SingleSelection);

	m_pGlTraceModel = model;

	m_pViewFilter = new GlTraceSettingsViewFilter(m_pGlTraceModel);
	m_pViewFilter->setSourceModel(m_pGlTraceModel);

	treeView->setModel(m_pViewFilter);

	connect(leSearch, SIGNAL(textChanged(const QString &)), m_pViewFilter,
			SLOT(setFilterWildcard(const QString &)));

	connect(buttonBox->button(QDialogButtonBox::RestoreDefaults),
			SIGNAL(pressed()), this, SLOT(resetToDefaults()));

	connect(buttonBox, SIGNAL(accepted()), this, SLOT(acceptSettings()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(rejectSettings()));

	connect(buttonBox->button(QDialogButtonBox::Reset), SIGNAL(pressed()), this,
			SLOT(rejectSettings()));
}

void GlTraceSettingsDialog::resetToDefaults()
{
	m_pGlTraceModel->resetToDefaults();
}

void GlTraceSettingsDialog::acceptSettings()
{
	m_pGlTraceModel->save();
}

void GlTraceSettingsDialog::rejectSettings()
{
	m_pGlTraceModel->load();
}
