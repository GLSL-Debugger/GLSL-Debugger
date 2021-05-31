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

#ifndef GL_TRACE_LIST_MODEL_H
#define GL_TRACE_LIST_MODEL_H

#include <QtCore/QTextStream>
#include <QtCore/QSortFilterProxyModel>
#include <QtCore/QAbstractListModel>
#include <QtCore/QString>
#include <QtGui/QIcon>

#include "glTraceFilterModel.qt.h"

class GlTraceListItem {
public:
	GlTraceListItem();
	~GlTraceListItem();

	enum IconType {
		IT_EMPTY,
		IT_ACTUAL,
		IT_OK,
		IT_ERROR,
		IT_WARNING,
		IT_IMPORTANT,
		IT_RECORD
	};

	void setIconType(IconType type);
	void setText(const QString &text)
	{
		m_qText = text;
	}

	QIcon getIcon(void) const
	{
		return m_qItem;
	}
	QString getText(void) const
	{
		return m_qText;
	}

	void outputTXT(QTextStream &out) const;

private:
	QIcon m_qItem;
	QString m_qText;
	IconType m_eIconType;
};

class GlTraceListFilterModel: public QSortFilterProxyModel {
public:
	GlTraceListFilterModel(GlTraceFilterModel *traceFilter,
			QObject *parent = 0);

protected:
	virtual bool filterAcceptsRow(int sourceRow,
			const QModelIndex &sourceParent) const;
	void showCurrentItemAnyway(bool show);

private:
	GlTraceFilterModel *m_GlTraceFilterModel;
	bool currentShown;
};

class GlTraceListModel: public QAbstractListModel {
public:
	GlTraceListModel(int maxListEntries, GlTraceFilterModel *traceFilter,
			QObject *parent = 0);
	~GlTraceListModel();

	void clear(void);
	void resetLayout(void)
	{
		layoutAboutToBeChanged();
		layoutChanged();
	}

	void addGlTraceItem(const GlTraceListItem::IconType type,
			const QString & text);
	void addGlTraceWarningItem(const QString & text);
	void addGlTraceErrorItem(const QString & text);

	void setCurrentGlTraceIconType(const GlTraceListItem::IconType type,
			int offset = 0);
	void setCurrentGlTraceText(const QString &text, int offset = 0);

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

	bool isCurrentCall(const QModelIndex &index);

	void traverse(QTextStream &out,
			void (GlTraceListItem::*outFunc)(QTextStream&) const);

private:
	int getNextIndex(void);
	int getLastRowIndex(void);

	GlTraceListItem *m_pData;
	int m_iMax;
	int m_iFirst;
	int m_iNum;
	GlTraceFilterModel *m_pTraceFilterModel;
};

#endif

