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

#include "watchTable.qt.h"
#include "vertexTableModel.qt.h"
#include "mappings.h"
#include "glScatter.qt.h"

#include "dbgprint.h"

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

static void clearData(float *data, int count, int dataStride, float clearValue)
{
    for (int i = 0; i < count; i++) {
        *data = clearValue;
        data += dataStride;
    }
}

WatchTable::WatchTable(QWidget *parent)
    : WatchView(parent)
{
    /* Setup GUI */
    setupUi(this);

    fMapping->setVisible(false);

    m_vertexTable = new VertexTableModel(this);

    m_filterProxy = new VertexTableSortFilterProxyModel(this);
    m_filterProxy->setSourceModel(m_vertexTable);
    m_filterProxy->setDynamicSortFilter(true);
    connect(tbHideInactive, SIGNAL(toggled(bool)), m_filterProxy, SLOT(setHideInactive(bool)));

    connect(m_vertexTable, SIGNAL(dataDeleted(int)), this, SLOT(detachData(int)));
    connect(m_vertexTable, SIGNAL(empty()), this, SLOT(closeView()));

    tvVertices->setModel(m_filterProxy);

    connect(tvVertices, SIGNAL(doubleClicked(const QModelIndex &)),
            this, SLOT(newSelection(const QModelIndex &)));

    // Add OpenGL view to window
    QGridLayout *gridLayout;
    gridLayout = new QGridLayout(fGLview);
    gridLayout->setSpacing(0);
    gridLayout->setMargin(0);
    m_qGLscatter = new GLScatter(this);
    gridLayout->addWidget(m_qGLscatter);

    slPointSize->setMinimum(1);
    slPointSize->setMaximum(1000);
    slPointSize->setValue(300);
    slPointSize->setTickInterval(50);

    m_scatterPositions = NULL;
    m_scatterColorsAndSizes = NULL;
    m_maxScatterDataElements = 0;
    m_scatterDataElements = 0;



    m_qGLscatter->setData(NULL, NULL, 0);
    on_slPointSize_valueChanged(300);

    setupMappingUI();
    updateGUI();
}

WatchTable::~WatchTable()
{
    m_qGLscatter->setData(NULL, NULL, 0);
    delete [] m_scatterPositions;
    delete [] m_scatterColorsAndSizes;
}

void WatchTable::setupMappingUI()
{
    /* Initialize color mapping comboboxes */
    Mapping m;
    m.index = 0;
    m.type  = MAP_TYPE_OFF;
    cbRed->addItem(QString("-"),
                QVariant(getIntFromMapping(m)));
    cbGreen->addItem(QString("-"),
                    QVariant(getIntFromMapping(m)));
    cbBlue->addItem(QString("-"),
                    QVariant(getIntFromMapping(m)));
    cbX->addItem(QString("-"),
                QVariant(getIntFromMapping(m)));
    cbY->addItem(QString("-"),
                QVariant(getIntFromMapping(m)));
    cbZ->addItem(QString("-"),
                QVariant(getIntFromMapping(m)));

    cbRed->setCurrentIndex(0);
    cbGreen->setCurrentIndex(0);
    cbBlue->setCurrentIndex(0);
    cbX->setCurrentIndex(0);
    cbY->setCurrentIndex(0);
    cbZ->setCurrentIndex(0);

    /* initialize range mapping comboboxes */
    RangeMapping rm;
    rm.index = 0;
    rm.range = RANGE_MAP_DEFAULT;
    cbMapRed->addItem(QIcon(QString::fromUtf8(":/icons/icons/watch-red-solid_32.png")),
                    QString(""),
                    QVariant(getIntFromRangeMapping(rm)));
    cbMapGreen->addItem(QIcon(QString::fromUtf8(":/icons/icons/watch-green-solid_32.png")),
                        QString(""),
                        QVariant(getIntFromRangeMapping(rm)));
    cbMapBlue->addItem(QIcon(QString::fromUtf8(":/icons/icons/watch-blue-solid_32.png")),
                    QString(""),
                    QVariant(getIntFromRangeMapping(rm)));
    cbMapX->addItem(QIcon(QString::fromUtf8(":/icons/icons/watch-black-solid_32.png")),
                    QString(""),
                    QVariant(getIntFromRangeMapping(rm)));
    cbMapY->addItem(QIcon(QString::fromUtf8(":/icons/icons/watch-black-solid_32.png")),
                    QString(""),
                    QVariant(getIntFromRangeMapping(rm)));
    cbMapZ->addItem(QIcon(QString::fromUtf8(":/icons/icons/watch-black-solid_32.png")),
                    QString(""),
                    QVariant(getIntFromRangeMapping(rm)));

    rm.index = 1;
    rm.range = RANGE_MAP_POSITIVE;
    cbMapRed->addItem(QIcon(QString::fromUtf8(":/icons/icons/watch-red-positive_32.png")),
                    QString(""),
                    QVariant(getIntFromRangeMapping(rm)));
    cbMapGreen->addItem(QIcon(QString::fromUtf8(":/icons/icons/watch-green-positive_32.png")),
                        QString(""),
                        QVariant(getIntFromRangeMapping(rm)));
    cbMapBlue->addItem(QIcon(QString::fromUtf8(":/icons/icons/watch-blue-positive_32.png")),
                    QString(""),
                    QVariant(getIntFromRangeMapping(rm)));
    cbMapX->addItem(QIcon(QString::fromUtf8(":/icons/icons/watch-black-positive_32.png")),
                    QString(""),
                    QVariant(getIntFromRangeMapping(rm)));
    cbMapY->addItem(QIcon(QString::fromUtf8(":/icons/icons/watch-black-positive_32.png")),
                    QString(""),
                    QVariant(getIntFromRangeMapping(rm)));
    cbMapZ->addItem(QIcon(QString::fromUtf8(":/icons/icons/watch-black-positive_32.png")),
                    QString(""),
                    QVariant(getIntFromRangeMapping(rm)));

    rm.index = 2;
    rm.range = RANGE_MAP_NEGATIVE;
    cbMapRed->addItem(QIcon(QString::fromUtf8(":/icons/icons/watch-red-negative_32.png")),
                    QString(""),
                    QVariant(getIntFromRangeMapping(rm)));
    cbMapGreen->addItem(QIcon(QString::fromUtf8(":/icons/icons/watch-green-negative_32.png")),
                        QString(""),
                        QVariant(getIntFromRangeMapping(rm)));
    cbMapBlue->addItem(QIcon(QString::fromUtf8(":/icons/icons/watch-blue-negative_32.png")),
                    QString(""),
                    QVariant(getIntFromRangeMapping(rm)));
    cbMapX->addItem(QIcon(QString::fromUtf8(":/icons/icons/watch-black-negative_32.png")),
                    QString(""),
                    QVariant(getIntFromRangeMapping(rm)));
    cbMapY->addItem(QIcon(QString::fromUtf8(":/icons/icons/watch-black-negative_32.png")),
                    QString(""),
                    QVariant(getIntFromRangeMapping(rm)));
    cbMapZ->addItem(QIcon(QString::fromUtf8(":/icons/icons/watch-black-negative_32.png")),
                    QString(""),
                    QVariant(getIntFromRangeMapping(rm)));

    rm.index = 3;
    rm.range = RANGE_MAP_ABSOLUTE;
    cbMapRed->addItem(QIcon(QString::fromUtf8(":/icons/icons/watch-red-absolute_32.png")),
                    QString(""),
                    QVariant(getIntFromRangeMapping(rm)));
    cbMapGreen->addItem(QIcon(QString::fromUtf8(":/icons/icons/watch-green-absolute_32.png")),
                        QString(""),
                        QVariant(getIntFromRangeMapping(rm)));
    cbMapBlue->addItem(QIcon(QString::fromUtf8(":/icons/icons/watch-blue-absolute_32.png")),
                    QString(""),
                    QVariant(getIntFromRangeMapping(rm)));
    cbMapX->addItem(QIcon(QString::fromUtf8(":/icons/icons/watch-black-absolute_32.png")),
                    QString(""),
                    QVariant(getIntFromRangeMapping(rm)));
    cbMapY->addItem(QIcon(QString::fromUtf8(":/icons/icons/watch-black-absolute_32.png")),
                    QString(""),
                    QVariant(getIntFromRangeMapping(rm)));
    cbMapZ->addItem(QIcon(QString::fromUtf8(":/icons/icons/watch-black-absolute_32.png")),
                    QString(""),
                    QVariant(getIntFromRangeMapping(rm)));

    cbMapRed->setCurrentIndex(0);
    cbMapGreen->setCurrentIndex(0);
    cbMapBlue->setCurrentIndex(0);
    cbMapX->setCurrentIndex(0);
    cbMapY->setCurrentIndex(0);
    cbMapZ->setCurrentIndex(0);
    on_cbRed_activated(0);
    on_cbGreen_activated(0);
    on_cbBlue_activated(0);
    on_cbX_activated(0);
    on_cbY_activated(0);
    on_cbZ_activated(0);
}

bool WatchTable::countsAllZero()
{
    if (m_scatterDataCountX != 0) {
        return false;
    }
    if (m_scatterDataCountY != 0) {
        return false;
    }
    if (m_scatterDataCountZ != 0) {
        return false;
    }
    if (m_scatterDataCountRed != 0) {
        return false;
    }
    if (m_scatterDataCountGreen != 0) {
        return false;
    }
    if (m_scatterDataCountBlue!= 0) {
        return false;
    }
    return true;
}

void WatchTable::updateDataCurrent(float *data, int *count, int dataStride,
                    VertexBox *srcData, Mapping *mapping,
                    RangeMapping *rangeMapping,
                    float min, float max)
{
    float minmax[2] = {min, fabs(max - min) > FLT_EPSILON ? max : 1.0f + min};
    float *dd = data;
    float *ds = srcData->getDataPointer();
    bool  *dc = srcData->getCoveragePointer();
    bool  *dm = srcData->getDataMapPointer();

    clearData(data, srcData->getNumVertices(), dataStride, 0.0f);

    *count = 0;
    for (int i = 0; i < srcData->getNumVertices(); i++) {
        if ((dc && !*dc) || (dm && !*dm)) {
        } else {
            *dd = getMappedValueF(*ds, mapping, rangeMapping, minmax);
            dd += dataStride;
            (*count)++;
        }
        ds++;
        if (dm) dm++;
        if (dc) dc++;
    }
    if (*count > 0) {
        m_scatterDataElements = *count;
    } else if (countsAllZero()) {
        m_scatterDataElements = 0;
    }

    m_qGLscatter->setData(m_scatterPositions, m_scatterColorsAndSizes,
                        m_scatterDataElements);
}

#define UPDATE_DATA(VAL) \
    if (cb##VAL->currentIndex() == 0) { \
        clearData(m_scatterData##VAL, m_maxScatterDataElements, \
                m_scatterDataStride##VAL, 0.0f); \
    } else { \
        Mapping m = getMappingFromInt(cb##VAL->itemData(cb##VAL->currentIndex()).toInt()); \
        RangeMapping rm = getRangeMappingFromInt(cbMap##VAL->itemData(cbMap##VAL->currentIndex()).toInt()); \
        updateDataCurrent(m_scatterData##VAL, &m_scatterDataCount##VAL, \
                        m_scatterDataStride##VAL, \
                        m_vertexTable->getDataColumn(m.index), &m, &rm, \
                        dsMin##VAL->value(), dsMax##VAL->value()); \
    }

void WatchTable::attachVpData(VertexBox *vb, QString name)
{
    if (!vb) {
        return;
    }


    if (m_vertexTable->addVertexBox(vb, name)) {
        if (!m_scatterPositions) {
            m_maxScatterDataElements = vb->getNumVertices();
            m_scatterDataElements = 0;
            m_scatterPositions = new float[3*m_maxScatterDataElements];
            m_scatterColorsAndSizes = new float[3*m_maxScatterDataElements];
            m_scatterDataX = m_scatterPositions;
            clearData(m_scatterDataX, m_maxScatterDataElements, 3, 0.0f);
            m_scatterDataCountX = 0;
            m_scatterDataY = m_scatterPositions + 1;
            clearData(m_scatterDataY, m_maxScatterDataElements, 3, 0.0f);
            m_scatterDataCountY = 0;
            m_scatterDataZ = m_scatterPositions + 2;
            clearData(m_scatterDataZ, m_maxScatterDataElements, 3, 0.0f);
            m_scatterDataCountZ = 0;
            m_scatterDataRed = m_scatterColorsAndSizes;
            clearData(m_scatterDataRed, m_maxScatterDataElements, 3, 0.0f);
            m_scatterDataCountRed = 0;
            m_scatterDataGreen = m_scatterColorsAndSizes + 1;
            clearData(m_scatterDataGreen, m_maxScatterDataElements, 3, 0.0f);
            m_scatterDataCountGreen = 0;
            m_scatterDataBlue = m_scatterColorsAndSizes + 2;
            clearData(m_scatterDataBlue, m_maxScatterDataElements, 3, 0.0f);
            m_scatterDataCountBlue = 0;
        }
        addMappingOptions(m_vertexTable->columnCount()-1);
        updateGUI();
    }
}

void WatchTable::detachData(int idx)
{
    dbgPrint(DBGLVL_DEBUG, "WatchTable::detachData: idx=%i\n", idx);
    delMappingOptions(idx);
}

void WatchTable::updateGUI()
{
    int i;

    QString title("");

    for (i = 0; i < m_vertexTable->columnCount(); i++) {
        title += m_vertexTable->headerData(i, Qt::Horizontal).toString();
        if (i != m_vertexTable->columnCount() - 1) {
            title += ", ";
        }
    }
    setWindowTitle(title);
}

void WatchTable::updateView(bool force)
{
    UNUSED_ARG(force)
    dbgPrint(DBGLVL_DEBUG, "WatchTable updateView\n");
    m_vertexTable->updateData();
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    update();
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
}

void WatchTable::closeView()
{
    hide();
    deleteLater();
}

void WatchTable::newSelection(const QModelIndex & index)
{
    emit selectionChanged(index.row());
}

void WatchTable::addMappingOptions(int idx)
{
    Mapping m;
    m.type = MAP_TYPE_VAR;
    m.index = idx;

    QString name = m_vertexTable->getDataColumnName(idx);
    cbRed->addItem(name, QVariant(getIntFromMapping(m)));
    cbGreen->addItem(name, QVariant(getIntFromMapping(m)));
    cbBlue->addItem(name, QVariant(getIntFromMapping(m)));
    cbX->addItem(name, QVariant(getIntFromMapping(m)));
    cbY->addItem(name, QVariant(getIntFromMapping(m)));
    cbZ->addItem(name, QVariant(getIntFromMapping(m)));
}

void WatchTable::delMappingOptions(int idx)
{
    /* Check if it's in use right now */
    QVariant data;
    Mapping  m;

    data = cbRed->itemData(cbRed->currentIndex());
    m    = getMappingFromInt(data.toInt());
    if (m.index == idx) {
        cbRed->setCurrentIndex(0);
        on_cbRed_activated(0);
    }
    data = cbGreen->itemData(cbGreen->currentIndex());
    m    = getMappingFromInt(data.toInt());
    if (m.index == idx) {
        cbGreen->setCurrentIndex(0);
        on_cbGreen_activated(0);
    }
    data = cbBlue->itemData(cbBlue->currentIndex());
    m    = getMappingFromInt(data.toInt());
    if (m.index == idx) {
        cbBlue->setCurrentIndex(0);
        on_cbBlue_activated(0);
    }
    data = cbX->itemData(cbX->currentIndex());
    m    = getMappingFromInt(data.toInt());
    if (m.index == idx) {
        cbX->setCurrentIndex(0);
        on_cbX_activated(0);
    }
    data = cbY->itemData(cbY->currentIndex());
    m    = getMappingFromInt(data.toInt());
    if (m.index == idx) {
        cbY->setCurrentIndex(0);
        on_cbY_activated(0);
    }
    data = cbZ->itemData(cbZ->currentIndex());
    m    = getMappingFromInt(data.toInt());
    if (m.index == idx) {
        cbZ->setCurrentIndex(0);
        on_cbZ_activated(0);
    }

    /* Delete options in comboboxes */
    int map;
    m.index = idx;

    m.type  = MAP_TYPE_VAR;
    map = getIntFromMapping(m);
    idx = cbRed->findData(QVariant(map));
    cbRed->removeItem(idx);
    for (int i = idx; i < cbRed->count(); i++) {
        m  = getMappingFromInt(cbRed->itemData(i).toInt());
        m.index--;
        cbRed->setItemData(i, getIntFromMapping(m));
    }
    idx = cbGreen->findData(QVariant(map));
    cbGreen->removeItem(idx);
    for (int i = idx; i < cbGreen->count(); i++) {
        m  = getMappingFromInt(cbGreen->itemData(i).toInt());
        m.index--;
        cbGreen->setItemData(i, getIntFromMapping(m));
    }
    idx = cbBlue->findData(QVariant(map));
    cbBlue->removeItem(idx);
    for (int i = idx; i < cbBlue->count(); i++) {
        m  = getMappingFromInt(cbBlue->itemData(i).toInt());
        m.index--;
        cbBlue->setItemData(i, getIntFromMapping(m));
    }
    idx = cbX->findData(QVariant(map));
    cbX->removeItem(idx);
    for (int i = idx; i < cbX->count(); i++) {
        m  = getMappingFromInt(cbX->itemData(i).toInt());
        m.index--;
        cbX->setItemData(i, getIntFromMapping(m));
    }
    idx = cbY->findData(QVariant(map));
    cbY->removeItem(idx);
    for (int i = idx; i < cbY->count(); i++) {
        m  = getMappingFromInt(cbY->itemData(i).toInt());
        m.index--;
        cbY->setItemData(i, getIntFromMapping(m));
    }
    idx = cbZ->findData(QVariant(map));
    cbZ->removeItem(idx);
    for (int i = idx; i < cbZ->count(); i++) {
        m  = getMappingFromInt(cbZ->itemData(i).toInt());
        m.index--;
        cbZ->setItemData(i, getIntFromMapping(m));
    }
    m_qGLscatter->updateGL();
}

#define UPDATE_MAPPING_MIN_MAX(VAL) \
    RangeMapping rm = getRangeMappingFromInt(cbMap##VAL->itemData(cbMap##VAL->currentIndex()).toInt()); \
    switch (rm.range) { \
        case RANGE_MAP_DEFAULT: \
            tbMin##VAL->setText(QString::number(m_vertexTable->getDataColumn(m.index)->getMin())); \
            tbMax##VAL->setText(QString::number(m_vertexTable->getDataColumn(m.index)->getMax())); \
            break; \
        case RANGE_MAP_POSITIVE: \
            tbMin##VAL->setText(QString::number(MAX(m_vertexTable->getDataColumn(m.index)->getMin(), 0.0))); \
            tbMax##VAL->setText(QString::number(MAX(m_vertexTable->getDataColumn(m.index)->getMax(), 0.0))); \
            break; \
        case RANGE_MAP_NEGATIVE: \
            tbMin##VAL->setText(QString::number(MIN(m_vertexTable->getDataColumn(m.index)->getMin(), 0.0))); \
            tbMax##VAL->setText(QString::number(MIN(m_vertexTable->getDataColumn(m.index)->getMax(), 0.0))); \
            break; \
        case RANGE_MAP_ABSOLUTE: \
            tbMin##VAL->setText(QString::number(m_vertexTable->getDataColumn(m.index)->getAbsMin())); \
            tbMax##VAL->setText(QString::number(m_vertexTable->getDataColumn(m.index)->getAbsMax())); \
            break; \
    }

#define ON_CB_ACTIVATED(VAL) \
    void WatchTable::on_cb##VAL##_activated(int newIdx) \
{ \
    Mapping m = getMappingFromInt(cb##VAL->itemData(cb##VAL->currentIndex()).toInt()); \
    if (newIdx == 0) { \
        tbMin##VAL->setEnabled(false); \
        tbMin##VAL->setText(QString::number(0.0)); \
        tbMax##VAL->setEnabled(false); \
        tbMax##VAL->setText(QString::number(0.0)); \
        dsMin##VAL->setEnabled(false); \
        dsMin##VAL->setValue(0.0); \
        dsMax##VAL->setEnabled(false); \
        dsMax##VAL->setValue(0.0); \
        cbSync##VAL->setEnabled(false); \
        cbMap##VAL->setEnabled(false); \
        cbMap##VAL->setCurrentIndex(0); \
        tbSwitch##VAL->setEnabled(false); \
        clearData(m_scatterData##VAL, m_maxScatterDataElements, \
                m_scatterDataStride##VAL, 0.0f); \
    } else { \
        /*RangeMapping rm = getRangeMappingFromInt(cbMap##VAL->itemData(cbMap##VAL->currentIndex()).toInt());*/ \
        tbMin##VAL->setEnabled(true); \
        tbMax##VAL->setEnabled(true); \
        dsMin##VAL->setEnabled(true); \
        dsMax##VAL->setEnabled(true); \
        cbSync##VAL->setEnabled(true); \
        cbMap##VAL->setEnabled(true); \
        tbSwitch##VAL->setEnabled(true); \
        UPDATE_MAPPING_MIN_MAX(VAL) \
        on_tbMin##VAL##_clicked(); \
        on_tbMax##VAL##_clicked(); \
        connect(m_vertexTable->getDataColumn(m.index), SIGNAL(dataChanged()), \
                this, SLOT(mappingDataChanged##VAL())); \
        updateDataCurrent(m_scatterData##VAL, &m_scatterDataCount##VAL,\
                m_scatterDataStride##VAL, \
                m_vertexTable->getDataColumn(m.index), &m, &rm, \
                dsMin##VAL->value(), dsMax##VAL->value()); \
    } \
    m_qGLscatter->updateGL(); \
    updateView(true); \
}

ON_CB_ACTIVATED(Red)
ON_CB_ACTIVATED(Green)
ON_CB_ACTIVATED(Blue)
ON_CB_ACTIVATED(X)
ON_CB_ACTIVATED(Y)
ON_CB_ACTIVATED(Z)

#define ON_CBMAP_ACTIVED(VAL) \
void WatchTable::on_cbMap##VAL##_activated(int newIdx) \
{ \
    UNUSED_ARG(newIdx) \
    Mapping m = getMappingFromInt(cb##VAL->itemData(cb##VAL->currentIndex()).toInt()); \
    UPDATE_MAPPING_MIN_MAX(VAL) \
    UPDATE_DATA(VAL) \
    m_qGLscatter->updateGL(); \
    updateView(true); \
}

ON_CBMAP_ACTIVED(Red)
ON_CBMAP_ACTIVED(Green)
ON_CBMAP_ACTIVED(Blue)
ON_CBMAP_ACTIVED(X)
ON_CBMAP_ACTIVED(Y)
ON_CBMAP_ACTIVED(Z)

#define ON_TBMIN_CLICKED(VAL) \
void WatchTable::on_tbMin##VAL##_clicked() \
{ \
    Mapping m = getMappingFromInt(cb##VAL->itemData(cb##VAL->currentIndex()).toInt()); \
    RangeMapping rm = getRangeMappingFromInt(cbMap##VAL->itemData(cbMap##VAL->currentIndex()).toInt()); \
    if (m.type == MAP_TYPE_VAR) { \
        switch (rm.range) { \
            case RANGE_MAP_DEFAULT: \
                dsMin##VAL->setValue(m_vertexTable->getDataColumn(m.index)->getMin()); \
                break; \
            case RANGE_MAP_POSITIVE: \
                dsMin##VAL->setValue(MAX(m_vertexTable->getDataColumn(m.index)->getMin(), 0.0)); \
                break; \
            case RANGE_MAP_NEGATIVE: \
                dsMin##VAL->setValue(MIN(m_vertexTable->getDataColumn(m.index)->getMin(), 0.0)); \
                break; \
            case RANGE_MAP_ABSOLUTE: \
                dsMin##VAL->setValue(m_vertexTable->getDataColumn(m.index)->getAbsMin()); \
                break; \
        } \
    } \
    UPDATE_DATA(VAL) \
    m_qGLscatter->updateGL(); \
}

ON_TBMIN_CLICKED(Red)
ON_TBMIN_CLICKED(Green)
ON_TBMIN_CLICKED(Blue)
ON_TBMIN_CLICKED(X)
ON_TBMIN_CLICKED(Y)
ON_TBMIN_CLICKED(Z)

#define ON_TBMAX_CLICKED(VAL) \
void WatchTable::on_tbMax##VAL##_clicked() \
{ \
    Mapping m = getMappingFromInt(cb##VAL->itemData(cb##VAL->currentIndex()).toInt()); \
    RangeMapping rm = getRangeMappingFromInt(cbMap##VAL->itemData(cbMap##VAL->currentIndex()).toInt()); \
    if (m.type == MAP_TYPE_VAR) { \
        switch (rm.range) { \
            case RANGE_MAP_DEFAULT: \
                dsMax##VAL->setValue(m_vertexTable->getDataColumn(m.index)->getMax()); \
                break; \
            case RANGE_MAP_POSITIVE: \
                dsMax##VAL->setValue(MAX(m_vertexTable->getDataColumn(m.index)->getMax(), 0.0)); \
                break; \
            case RANGE_MAP_NEGATIVE: \
                dsMax##VAL->setValue(MIN(m_vertexTable->getDataColumn(m.index)->getMax(), 0.0)); \
                break; \
            case RANGE_MAP_ABSOLUTE: \
                dsMax##VAL->setValue(m_vertexTable->getDataColumn(m.index)->getAbsMax()); \
                break; \
        } \
    } \
    UPDATE_DATA(VAL) \
    m_qGLscatter->updateGL(); \
}

ON_TBMAX_CLICKED(Red)
ON_TBMAX_CLICKED(Green)
ON_TBMAX_CLICKED(Blue)
ON_TBMAX_CLICKED(X)
ON_TBMAX_CLICKED(Y)
ON_TBMAX_CLICKED(Z)

#define ON_DSMIN_VALUECHANGED(VAL) \
void WatchTable::on_dsMin##VAL##_valueChanged(double d)\
{ \
    UNUSED_ARG(d) \
    UPDATE_DATA(VAL) \
    m_qGLscatter->updateGL(); \
    updateView(true); \
}

ON_DSMIN_VALUECHANGED(Red)
ON_DSMIN_VALUECHANGED(Green)
ON_DSMIN_VALUECHANGED(Blue)
ON_DSMIN_VALUECHANGED(X)
ON_DSMIN_VALUECHANGED(Y)
ON_DSMIN_VALUECHANGED(Z)

#define ON_DSMAX_VALUECHANGED(VAL) \
void WatchTable::on_dsMax##VAL##_valueChanged(double d) \
{ \
    UNUSED_ARG(d) \
    UPDATE_DATA(VAL) \
    m_qGLscatter->updateGL(); \
    updateView(true); \
}

ON_DSMAX_VALUECHANGED(Red)
ON_DSMAX_VALUECHANGED(Green)
ON_DSMAX_VALUECHANGED(Blue)
ON_DSMAX_VALUECHANGED(X)
ON_DSMAX_VALUECHANGED(Y)
ON_DSMAX_VALUECHANGED(Z)

#define ON_TBSWITCH_CLICKED(VAL) \
void WatchTable::on_tbSwitch##VAL##_clicked() \
{ \
    float tmp = dsMin##VAL->value(); \
    dsMin##VAL->setValue(dsMax##VAL->value()); \
    dsMax##VAL->setValue(tmp); \
    UPDATE_DATA(VAL) \
    m_qGLscatter->updateGL(); \
    updateView(true); \
}

ON_TBSWITCH_CLICKED(Red)
ON_TBSWITCH_CLICKED(Green)
ON_TBSWITCH_CLICKED(Blue)
ON_TBSWITCH_CLICKED(X)
ON_TBSWITCH_CLICKED(Y)
ON_TBSWITCH_CLICKED(Z)

#define MAPPING_DATA_CHANGED(VAL) \
void WatchTable::mappingDataChanged##VAL() \
{ \
    VertexBox *sendingVB = static_cast<VertexBox*>(sender()); \
    Mapping m = getMappingFromInt(cb##VAL->itemData(cb##VAL->currentIndex()).toInt()); \
    if (sendingVB) { \
        VertexBox *activeVB = NULL; \
        activeVB = m_vertexTable->getDataColumn(m.index); \
        if (activeVB != sendingVB) { \
            disconnect(sendingVB, SIGNAL(dataChanged()), this, SLOT(mappingDataChanged##VAL())); \
            return; \
        } \
    } \
    RangeMapping rm = getRangeMappingFromInt(cbMap##VAL->itemData(cbMap##VAL->currentIndex()).toInt()); \
    switch (rm.range) { \
        case RANGE_MAP_DEFAULT: \
            tbMin##VAL->setText(QString::number(m_vertexTable->getDataColumn(m.index)->getMin())); \
            tbMax##VAL->setText(QString::number(m_vertexTable->getDataColumn(m.index)->getMax())); \
            break; \
        case RANGE_MAP_POSITIVE: \
            tbMin##VAL->setText(QString::number(MAX(m_vertexTable->getDataColumn(m.index)->getMin(), 0.0))); \
            tbMax##VAL->setText(QString::number(MAX(m_vertexTable->getDataColumn(m.index)->getMax(), 0.0))); \
            break; \
        case RANGE_MAP_NEGATIVE: \
            tbMin##VAL->setText(QString::number(MIN(m_vertexTable->getDataColumn(m.index)->getMin(), 0.0))); \
            tbMax##VAL->setText(QString::number(MIN(m_vertexTable->getDataColumn(m.index)->getMax(), 0.0))); \
            break; \
        case RANGE_MAP_ABSOLUTE: \
            tbMin##VAL->setText(QString::number(m_vertexTable->getDataColumn(m.index)->getAbsMin())); \
            tbMax##VAL->setText(QString::number(m_vertexTable->getDataColumn(m.index)->getAbsMax())); \
            break; \
    } \
    updateDataCurrent(m_scatterData##VAL, &m_scatterDataCount##VAL, m_scatterDataStride##VAL, \
                    sendingVB, &m, &rm, dsMin##VAL->value(), dsMax##VAL->value()); \
    m_qGLscatter->updateGL(); \
}

MAPPING_DATA_CHANGED(Red)
MAPPING_DATA_CHANGED(Green)
MAPPING_DATA_CHANGED(Blue)
MAPPING_DATA_CHANGED(X)
MAPPING_DATA_CHANGED(Y)
MAPPING_DATA_CHANGED(Z)

void WatchTable::on_slPointSize_valueChanged(int value)
{
    float newValue = 0.03*(float)value/(float)slPointSize->maximum();
    lbPointSize->setText(QString::number(newValue));
    m_qGLscatter->setPointSize(newValue);
    m_qGLscatter->updateGL();
}

