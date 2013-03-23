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

#ifndef GL_TRACE_FILTER_MODEL_H
#define GL_TRACE_FILTER_MODEL_H

#include <QtGui/QItemEditorCreatorBase>
#include <QtCore/QAbstractItemModel>
#include <QtCore/QVariant>
#include <QtCore/QSettings>
#include <QtCore/QList>
#include <QtCore/QHash>

#include "debuglib.h"

class GlTraceFilterModel : public QAbstractItemModel {
public:
	GlTraceFilterModel(GLFunctionList *functions, QObject *parent = 0);
	~GlTraceFilterModel();

	QModelIndex index (int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent (const QModelIndex &index) const;
	int rowCount (const QModelIndex &parent = QModelIndex()) const;
	int columnCount (const QModelIndex &parent = QModelIndex()) const;
	QVariant data (const QModelIndex &index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	Qt::ItemFlags flags (const QModelIndex &index) const;
	bool setData (const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
	bool isFunctionVisible(const QString &fname);
	//bool hasChildren (const QModelIndex &parent = QModelIndex()) const;

	enum columnName {FUNCTION_NAME, NUM_COLUMNS};

    void resetToDefaults(void);

    bool itemContainsPatternRecursive(const QModelIndex &index, 
                                      const QString &pattern);

    void save(void);
    void load(void);

private:
	class GlTraceFilterItem {
		public:
			  GlTraceFilterItem(GLFunctionList *theFunction, int show, 
                                GlTraceFilterItem *parent = 0);
			 ~GlTraceFilterItem();

			 void appendChild(GlTraceFilterItem *child);

			 void checkChildsToggleState();
			 void setChildsToggleState(int newState);
			 void setChildsToggleStateRecursive(int newState);

             bool containsFilterPatternRecursive(const QString &pattern);

			 GlTraceFilterItem *child(int row);
			 int childCount() const;
			 int columnCount() const;
			 QVariant data(int column) const;
			 int row() const;
			 GlTraceFilterItem *parent();

             void saveRecursive(QSettings &settings);
             void loadRecursive(QSettings &settings);

			 int showInTrace;
			 bool isSupported;
			 GLFunctionList *function;

		 private:
			 QList<GlTraceFilterItem*> childItems;
			 GlTraceFilterItem *parentItem;
	};

	void checkExtensions();
	void constructHashMap();

	GlTraceFilterItem *rootItem;
	QHash<QString, GlTraceFilterItem *> functionHash;
};

#endif
