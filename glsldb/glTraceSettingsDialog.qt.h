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

#ifndef GL_TRACE_SETTINGS_DIALOG_H
#define GL_TRACE_SETTINGS_DIALOG_H

#include <QtGui/QItemEditorCreatorBase>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QCheckBox>
#include <QtCore/QString>

#include "ui/glTraceSettingsDialog.ui.h"
#include "glTraceFilterModel.qt.h"

class GlTraceSettingsViewFilter : public QSortFilterProxyModel
{
    Q_OBJECT

    public:
        GlTraceSettingsViewFilter(QObject *parent = 0);

    public slots:
        void setFilterWildcard(const QString &pattern);

    protected:
        virtual bool filterAcceptsRow(int sourceRow, 
                const QModelIndex &sourceParent) const;

    private:
        QString filterPattern;
};


class GlTraceSettingsDialog : public QDialog, public Ui::dGlTraceSettings
{
    Q_OBJECT

    public:
        GlTraceSettingsDialog(GlTraceFilterModel *model, QWidget *parent=0);

    private slots:
        void resetToDefaults();
        void acceptSettings();
        void rejectSettings();

	private:
		void getExtensions();

        GlTraceSettingsViewFilter *m_pViewFilter;
        GlTraceFilterModel *m_pGlTraceModel;

};

#endif
