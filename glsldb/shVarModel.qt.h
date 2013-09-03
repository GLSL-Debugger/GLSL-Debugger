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

#ifndef _SH_VAR_MODEL_QT_H_
#define _SH_VAR_MODEL_QT_H_

#include <QtCore/QList>
#include <QtCore/QVariant>
#include <QtGui/QTreeView>
#include <QtGui/QTableView>
#include <QtGui/QAbstractItemView>
#include <QtCore/QAbstractItemModel>
#include <QtGui/QSortFilterProxyModel>
#include <QtCore/QCoreApplication>

#include "ShaderLang.h"

#include "pixelBox.qt.h"
#include "vertexBox.qt.h"

class UniformData
{
	public:
		virtual ~UniformData() {};
		virtual QString toString(int element) const = 0;
};

template <typename T>
class TypedUniformData : public UniformData
{
	public:
		TypedUniformData(const char * const data, int numElements)
		{
			m_data = new T[numElements];
			memcpy(m_data, data, sizeof(T)*numElements);
			m_numElements = numElements;
		}

		~TypedUniformData()
		{
			delete [] m_data;
		}

		QString toString(int element) const
		{
			if (element < m_numElements) {
				return QString::number(m_data[element]);
			} else {
				return "?";
			}

		}

	private:
		T* m_data;
		int m_numElements;
};

class Uniform
{
	public:

		Uniform();
		~Uniform();

		int initialize(const char* serializedUniform);

		QString name() const
		{
			return m_name;
		}

		int arraySize() const
		{
			return m_arraySize;
		}

		bool isVector() const
		{
			return m_isVector;
		}

		bool isMatrix() const
		{
			return m_isMatrix;
		}

		int rows() const
		{
			return m_rows;
		}

		int columns() const
		{
			return m_columns;
		}

		QString toString() const;

		QString toString(int arrayElement) const;

	private:
		QString m_name;
		int m_arraySize;
		bool m_isVector;
		bool m_isMatrix;
		int m_rows;
		int m_columns;
		UniformData* m_data;
};

class ShVarItem
{
public:
    ShVarItem(const QList<QVariant> &i_qData, ShVarItem *i_qParent = 0);
    ~ShVarItem();

    void appendChild(ShVarItem *i_qChild);

    ShVarItem *child(int i_nRow);
    ShVarItem *parent();

    int childCount() const;
    int columnCount() const;
    int row() const;

    QVariant data(int column) const;
    void setData(int i_nColumn, QVariant i_qData);

    ShChangeable* getShChangeable(void);

	void       setPixelBoxPointer(PixelBox *fb);
    PixelBox*  getPixelBoxPointer(void);

	void       setVertexBoxPointer(VertexBox *vb);
	VertexBox* getVertexBoxPointer(void);

	void       setCurrentPointer(VertexBox *vb);
	VertexBox* getCurrentPointer(void);

    bool      isChanged(void);
    bool      isSelectable(void);
	bool      isInScope(void);
	bool      isInScopeStack(void);
	bool      hasEnteredScope(void);
	bool      hasLeftScope(void);
	bool      isBuildIn(void);
	bool      isUniform(void);
	bool      isActiveUniform(void);
    QString   getFullName(void);
	int getReadbackFormat();

	void setCurrentValue(int key0, int key1, int key2);
	void setCurrentValue(int key0, int key1);
	void setCurrentValue(int key0);
	void resetCurrentValue(void);

	enum Scope {NewInScope, InScope, InScopeStack, LeftScope, OutOfScope};

private:

    QList<ShVarItem*> m_qChilds;
    QList<QVariant>   m_qData;
    ShVarItem        *m_qParent;
};

class ShVarModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    ShVarModel(ShVariableList *i_pVL, QObject *i_qParent, QCoreApplication *i_qApp);
    ~ShVarModel();

    QModelIndex index(int i_nRow, int i_nColumn, const QModelIndex &i_qParent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &i_qIndex) const;
    int rowCount(const QModelIndex &i_qParent) const;
    int columnCount(const QModelIndex &i_qParent) const;
    QVariant data(const QModelIndex &i_qIndex, int i_nRole) const;
    Qt::ItemFlags flags(const QModelIndex &i_qIndex) const;
    QVariant headerData(int i_nSection, Qt::Orientation i_qOrientation, int i_nRole) const;

    void setChangedAndScope(ShChangeableList &i_pCL, DbgRsScope &i_pSL, DbgRsScope &i_pSSL);

	void setCurrentValues(int key0, int key1);
	void setCurrentValues(int key0);
	void resetCurrentValues(void);
	void currentValuesChanged(void);

	void setUniformValues(char *pSerializedUniforms, int numUniforms);
	void setRecursiveUniformValues(ShVarItem *item, const Uniform &u);

    typedef enum {
        TV_ALL,
        TV_BUILTIN,
        TV_SCOPE,
        TV_WATCH_LIST,
		TV_UNIFORM
    } tvDisplayRole;

    void attach(QTreeView *view, tvDisplayRole role);
    void detach(QAbstractItemView *view);

    void unsetItemWatched(const QModelIndex & i_qIndex);
    ShVarItem* getWatchItemPointer(const QModelIndex & i_qIndex);
    QList<ShVarItem*> getAllWatchItemPointers(void);
    QList<ShVarItem*> getChangedWatchItemPointers(void);

    QSortFilterProxyModel* getFilterModel(tvDisplayRole role);

signals:
    void newWatchItem(ShVarItem*);

private slots:
    void onDoubleClickedAll(const QModelIndex & index);
    void onDoubleClickedBuiltIns(const QModelIndex & index);
    void onDoubleClickedScope(const QModelIndex & index);
    void onDoubleClickedUniforms(const QModelIndex & index);

private:
    QModelIndex getIndex(ShVarItem *i_qItem, int i_nColumn);

    void setupModelData(const ShVariableList *i_pVL, ShVarItem *i_qParent);

    void setRecursiveWatched(ShVarItem *i_qItem);
    void setRecursiveExpand(ShVarItem *i_qItem);
    void setItemWatched(const QModelIndex & i_qIndex);
    void unsetRecursiveWatched(ShVarItem *i_qItem);

    void clearChanged(ShVarItem *i_qItem);
    void setRecursiveScope(ShVarItem *i_qItem, ShVarItem::Scope scope);
    void setRecursiveChanged(ShVarItem *i_qItem);

    QCoreApplication *m_qApp;
    ShVarItem *m_qRootItem;

    QSortFilterProxyModel *m_qAllProxy;
    QSortFilterProxyModel *m_qBuiltInProxy;
    QSortFilterProxyModel *m_qScopeProxy;
    QSortFilterProxyModel *m_qUniformProxy;
    QSortFilterProxyModel *m_qWatchProxy;

    QTreeView *m_qWatchList;

    QList<ShVarItem*>      m_qWatchListItems;
};

class ScopeSortFilterProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

	public:

	ScopeSortFilterProxyModel(QObject *parent = 0);

	protected:

	virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
};

class SamplerSortFilterProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

	public:

	SamplerSortFilterProxyModel(QObject *parent = 0);

	protected:

	virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
};

class UniformSortFilterProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

	public:

	UniformSortFilterProxyModel(QObject *parent = 0);

	protected:

	virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
};

#endif
