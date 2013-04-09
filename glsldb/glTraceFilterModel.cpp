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

#include "glTraceFilterModel.qt.h"

#undef CursorShape
#include <QtGui/QCursor>
#include <QtCore/QStringList>
#include <QtGui/QIcon>
#include <QtOpenGL/QGLWidget>
#include <QtOpenGL/QGLContext>
#include <QtOpenGL/QGLFormat>
#include <QtCore/QSettings>


void GlTraceFilterModel::GlTraceFilterItem::appendChild(GlTraceFilterModel::GlTraceFilterItem *item) {
	childItems.append(item);
}

void GlTraceFilterModel::GlTraceFilterItem::checkChildsToggleState()
{
	int i;

	if (childItems.count() == 0) {
		return;
	}

	if (childItems.count() == 1) {
		showInTrace = childItems[0]->showInTrace;
		return;
	}

	for (i=1; i<childItems.count(); i++) {
		if (childItems[i-1]->showInTrace != childItems[i]->showInTrace) {
			showInTrace = Qt::PartiallyChecked;
			return;
		}
	}

	showInTrace = childItems[i-1]->showInTrace;
}

void GlTraceFilterModel::GlTraceFilterItem::setChildsToggleState(int newState)
{
	int i;

	for (i=0; i<childItems.count(); i++) {
		childItems[i]->showInTrace = newState;
	}
}

void GlTraceFilterModel::GlTraceFilterItem::setChildsToggleStateRecursive(int newState)
{
    int i;
    for (i=0; i<childItems.count(); i++) {
        childItems[i]->showInTrace = newState;
        childItems[i]->setChildsToggleStateRecursive(newState);
    }
}


GlTraceFilterModel::GlTraceFilterItem::GlTraceFilterItem(GLFunctionList *theFunction, int show,
		GlTraceFilterItem *parent) {
	showInTrace = show;
	function = theFunction;
	parentItem = parent;
	isSupported = false;
}

GlTraceFilterModel::GlTraceFilterItem::~GlTraceFilterItem() {
	qDeleteAll(childItems);
}


GlTraceFilterModel::GlTraceFilterItem *GlTraceFilterModel::GlTraceFilterItem::child(int row) {
	return childItems.value(row);
}

int GlTraceFilterModel::GlTraceFilterItem::childCount() const {
	return childItems.count();
}

int GlTraceFilterModel::GlTraceFilterItem::columnCount() const {
	return NUM_COLUMNS;
}

QVariant GlTraceFilterModel::GlTraceFilterItem::data(int column) const {
	switch(column) {
		case FUNCTION_NAME:
			if (this->parentItem != NULL) {
				if (this->parentItem->function == NULL) {
					return function->extname;
				} else {
					return function->fname;
				}
			} else {
				return QVariant();
			}
			break; 
		default:
			return QVariant();
	}
}

GlTraceFilterModel::GlTraceFilterItem *GlTraceFilterModel::GlTraceFilterItem::parent() {
	return parentItem;
}

int GlTraceFilterModel::GlTraceFilterItem::row() const {
	if (parentItem)
		return parentItem->childItems.indexOf(const_cast<GlTraceFilterItem*>(this));

	return 0;
}



GlTraceFilterModel::GlTraceFilterModel(GLFunctionList *functions, QObject *parent)
	: QAbstractItemModel(parent) {

	rootItem = new GlTraceFilterItem(NULL, true, 0);

	QString currExtension;
	int i = 0;

	GlTraceFilterItem *tfiExtension = NULL, *tfiChild;
	
	while(functions[i].extname != NULL) {
		if (currExtension.compare(QString(functions[i].extname))) {
			currExtension = QString(functions[i].extname);
			tfiExtension = new GlTraceFilterItem(&functions[i], Qt::Checked, rootItem);
			rootItem->appendChild(tfiExtension);
		}
		tfiChild = new GlTraceFilterItem(&functions[i], Qt::Checked, tfiExtension);
		tfiExtension->appendChild(tfiChild);
		i++;
	}

	this->checkExtensions();
	this->constructHashMap();

    load();
}

GlTraceFilterModel::~GlTraceFilterModel() {
	delete rootItem;
}

void GlTraceFilterModel::resetToDefaults(void)
{
    if (rootItem) {
        rootItem->setChildsToggleStateRecursive(Qt::Checked);
        layoutChanged();
    }
}

bool GlTraceFilterModel::itemContainsPatternRecursive(
        const QModelIndex &index, const QString &pattern)
{
    if (index.isValid()) {

        GlTraceFilterItem *item = 
            static_cast<GlTraceFilterItem*>(index.internalPointer());

        return item->containsFilterPatternRecursive(pattern);
    }

    return true;
}

bool GlTraceFilterModel::GlTraceFilterItem::containsFilterPatternRecursive(
        const QString &pattern)
{
    int i;
    if (parentItem->function == NULL) {
        if (function) {
            if (QString(function->extname).contains(pattern, 
                                                    Qt::CaseInsensitive)) {
                return true;
            }
            for (i=0; i<childCount(); i++) {
                if (childItems[i]->containsFilterPatternRecursive(pattern)) {
                    return true;
                }
            }
            return false;
        }
    } else {
        if (function) {
            return QString(function->fname).contains(pattern, 
                                                     Qt::CaseInsensitive);
        }
    }
    return true;
}

void GlTraceFilterModel::save(void)
{
    QSettings settings;

    settings.beginGroup("GLTRACE/");

    if (rootItem) {
        rootItem->saveRecursive(settings);
    }

    settings.endGroup();
}

void GlTraceFilterModel::GlTraceFilterItem::saveRecursive(QSettings &settings)
{
    int i;
    QString   key = QString("");

    if (parentItem) {
        if (parentItem->function == NULL) {
            /* Extension descriptor */
            key.append(function->extname);
            settings.setValue(key, showInTrace);
            for ( i=0; i<childCount(); i++) {
                childItems[i]->saveRecursive(settings);
            }
        } else {
            /* Function descriptor */
            key.append(parentItem->function->extname);
            key.append("/");
            key.append(function->fname);
            settings.setValue(key, showInTrace);
        }
    } else {
        for ( i=0; i<childCount(); i++) {
            childItems[i]->saveRecursive(settings);
        }
    }
}

void GlTraceFilterModel::load(void)
{
    QSettings settings;

    settings.beginGroup("GLTRACE/");

    if (rootItem) {
        rootItem->loadRecursive(settings);
    }

    settings.endGroup();
        
    layoutChanged();
}

void GlTraceFilterModel::GlTraceFilterItem::loadRecursive(QSettings &settings)
{
    int i;
    QString   key = QString("");

    if (parentItem) {
        if (parentItem->function == NULL) {
            /* Extension descriptor */
            key.append(function->extname);
            if (settings.contains(key)) {
                showInTrace = settings.value(key).toInt();
            } else {
                showInTrace = Qt::Checked;
            }

            for ( i=0; i<childCount(); i++) {
                childItems[i]->loadRecursive(settings);
            }
        } else {
            /* Function descriptor */
            key.append(parentItem->function->extname);
            key.append("/");
            key.append(function->fname);

            if (settings.contains(key)) {
            showInTrace = settings.value(key).toInt();
            } else {
                showInTrace = Qt::Checked;
            }
        }
    } else {
        for ( i=0; i<childCount(); i++) {
            childItems[i]->loadRecursive(settings);
        }
    }
}

QModelIndex GlTraceFilterModel::index (int row, int column, const QModelIndex &parent) const {
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	GlTraceFilterItem *parentItem;

	if (!parent.isValid())
		parentItem = rootItem;
	else
		parentItem = static_cast<GlTraceFilterItem*>(parent.internalPointer());

	GlTraceFilterItem *childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();
}

QModelIndex GlTraceFilterModel::parent (const QModelIndex &index) const {
	if (!index.isValid())
		return QModelIndex();

	GlTraceFilterItem *childItem = static_cast<GlTraceFilterItem*>(index.internalPointer());
	GlTraceFilterItem *parentItem = childItem->parent();

	if (parentItem == rootItem)
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}

int GlTraceFilterModel::rowCount (const QModelIndex &parent) const {
	GlTraceFilterItem *parentItem;
	if (parent.column() > 0)
		return 0;

	if (!parent.isValid())
		parentItem = rootItem;
	else
		parentItem = static_cast<GlTraceFilterItem*>(parent.internalPointer());

	return parentItem->childCount();
}

int GlTraceFilterModel::columnCount (const QModelIndex &parent) const {
	if (parent.isValid())
		return static_cast<GlTraceFilterItem*>(parent.internalPointer())->columnCount();
	else
		return rootItem->columnCount();
}

QVariant GlTraceFilterModel::data (const QModelIndex &index, int role) const {
	if (!index.isValid())
		return QVariant();

	GlTraceFilterItem *item = static_cast<GlTraceFilterItem*>(index.internalPointer());

	switch(index.column()) {
		case FUNCTION_NAME:
			if (role == Qt::DisplayRole) {
				return item->data(index.column());
			}
			if (role == Qt::DecorationRole) {
				if (item->parent() == rootItem) {
					if (item->isSupported) {
						return QIcon(QString::fromUtf8(":/icons/icons/dialog-ok_32.png"));
					} else {
						return QIcon(QString::fromUtf8(":/icons/icons/process-stop_32.png"));
					}
				} else {
					return QVariant();
				}
			}
			if (role == Qt::CheckStateRole) {
				switch (item->showInTrace) {
					case 0:
						return Qt::Unchecked;
					case 1:
						return Qt::PartiallyChecked;
					case 2:
						return Qt::Checked;
					default:
						return QVariant();
				}
			}
			break;
	}
	return QVariant();
}

QVariant GlTraceFilterModel::headerData(int section, Qt::Orientation orientation,
		int role) const {
	
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
		switch(section) {
			case FUNCTION_NAME:
				return QString("Function Name");
				break;
			default:
				return QVariant();
		}
	}
	return QVariant();
}

Qt::ItemFlags GlTraceFilterModel::flags (const QModelIndex &index) const {
	if (!index.isValid()) {
		return Qt::ItemIsEnabled;
	}
	Qt::ItemFlags f = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	if (index.column() == 0) {
		f |= Qt::ItemIsUserCheckable | Qt::ItemIsTristate;
	}
	return f;
}

bool GlTraceFilterModel::setData (const QModelIndex & index, const QVariant & value, int role) {
	if (index.isValid() && role == Qt::CheckStateRole) {
		switch(index.column()) {
			case FUNCTION_NAME:
				(static_cast<GlTraceFilterItem*>(index.internalPointer()))->showInTrace = value.toInt();

				if (index.parent().isValid()) {
					(static_cast<GlTraceFilterItem*>(index.parent().internalPointer()))->checkChildsToggleState();
					emit dataChanged(index.parent(),index.parent());
				} else {
					(static_cast<GlTraceFilterItem*>(index.internalPointer()))->setChildsToggleState(value.toInt());
                    layoutChanged();
				}
				return true;
				break;
		}
	}
	return false;
}

void GlTraceFilterModel::checkExtensions() {
	const GLubyte *extensionString, *versionString;
	int supportedMajor, supportedMinor;

	QGLWidget w;

	w.makeCurrent();
	extensionString = glGetString(GL_EXTENSIONS);
	versionString = glGetString(GL_VERSION);
	supportedMajor = versionString[0] - '0';
	supportedMinor = versionString[2] - '0';


	QByteArray extensions = QByteArray(reinterpret_cast<const char*>(extensionString));

#ifdef GLSLDB_WIN32
	PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB = 0;
	wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");
	if(wglGetExtensionsStringARB) {
		extensions.append(' ');
		extensions.append(QByteArray(reinterpret_cast<const char*>(wglGetExtensionsStringARB(wglGetCurrentDC()))));
	}
#elif defined(GLSLDB_LINUX)
    int supportedXMajor, supportedXMinor;
    const char *versionXString;

    versionXString = glXQueryServerString(XOpenDisplay(NULL), 0, GLX_VERSION);

    supportedXMajor = versionXString[0] - '0';
    supportedXMinor = versionXString[2] - '0';

    extensions.append(' ');
    extensions.append(QByteArray(reinterpret_cast<const char*>(glXQueryServerString(XOpenDisplay(NULL), 
                                                                                    0, GLX_EXTENSIONS))));

    extensions.append(' ');
    extensions.append(QByteArray(reinterpret_cast<const char*>(glXGetClientString(XOpenDisplay(NULL), 
                                                                                  GLX_EXTENSIONS))));

    extensions.append(' ');
    extensions.append(QByteArray(reinterpret_cast<const char*>(glXQueryExtensionsString(XOpenDisplay(NULL), 0))));
#elif defined(GLSLDB_OSX)

#warning "FIXME: any OSX specific extensions wee need to add here?"

#endif

	QList<QByteArray> extList = extensions.split(' ');
	for(int i = 0; i < this->rootItem->childCount(); i++) {
		GlTraceFilterItem *item = this->rootItem->child(i);
		if (strstr(item->function->extname, "GL_VERSION_") == item->function->extname) {
			int major, minor;
			major = item->function->extname[11] - '0';
			minor = item->function->extname[13] - '0';
			if (major < supportedMajor || (major == supportedMajor && minor <= supportedMinor)) {
				item->isSupported = true;
			} else {
				item->isSupported = false;
			}
		}
#if defined(_WIN32)
		else if (strstr(item->function->extname, "WGL_VERSION_") == item->function->extname) {
			item->isSupported = true;
		}
#elif defined(GLSLDB_LINUX)           
		else if (strstr(item->function->extname, "GLX_VERSION_") == item->function->extname) {
			int major, minor;
			major = item->function->extname[12] - '0';
			minor = item->function->extname[14] - '0';
			if (major < supportedXMajor || (major == supportedXMajor && minor <= supportedXMinor)) {
				item->isSupported = true;
			} else {
				item->isSupported = false;
            }
        }
#elif defined(GLSLDB_OSX)
#warning "FIXME: any OSX specific extensions wee need to add here?"	
#endif
		else {
			item->isSupported = extList.contains(item->function->extname);
		}
	}
}

void GlTraceFilterModel::constructHashMap() {
	for(int i = 0; i < this->rootItem->childCount(); i++) {
		GlTraceFilterItem *item = this->rootItem->child(i);
		for(int j = 0; j < item->childCount(); j++) {
			GlTraceFilterItem *subItem = item->child(j);
			functionHash[QString(subItem->function->fname)] = subItem;
		}
	}
}

bool GlTraceFilterModel::isFunctionVisible(const QString &fname) {
	if (functionHash[fname] != NULL) {
		return (functionHash[fname]->showInTrace == Qt::Checked);
	}
	return true;
}

//bool GlTraceFilterModel::hasChildren (const QModelIndex &parent) const {
//	if (parent.isValid())
//		return parent.
//}


