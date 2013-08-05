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

#include <math.h>
#include <float.h>

#include "watchGeoDataTree.qt.h"
#include "geoShaderDataModel.qt.h"
#include "mappings.h"
#include "dbgprint.h"

extern "C" {
#include "glenumerants/glenumerants.h"
}

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

static void clearData(float *data, int count, int dataStride, float clearValue)
{
    for (int i = 0; i < count; i++) {
        *data = clearValue;
        data += dataStride;
    }
}

WatchGeoDataTree::WatchGeoDataTree(int inPrimitiveType, int outPrimitiveType,
                                VertexBox *primitiveMap, VertexBox *vertexCount,
                                QWidget *parent)
    : WatchView(parent)
{
    /* Setup GUI */
    setupUi(this);

    fMapping->setVisible(false);

    m_dataModel = new GeoShaderDataModel(inPrimitiveType, outPrimitiveType,
                                        primitiveMap, vertexCount, NULL, NULL, this);
    m_filterProxy = new GeoShaderDataSortFilterProxyModel(this);
    m_filterProxy->setSourceModel(m_dataModel);
    m_filterProxy->setDynamicSortFilter(true);
    connect(tbHideInactive, SIGNAL(toggled(bool)), m_filterProxy, SLOT(setHideInactive(bool)));
    connect(tbHideEmpty, SIGNAL(toggled(bool)), m_filterProxy, SLOT(setHideEmpty(bool)));
    tvGeoData->setModel(m_filterProxy);
    tvGeoData->setAllColumnsShowFocus(true);
    tvGeoData->setUniformRowHeights(true);

    connect(tvGeoData, SIGNAL(doubleClicked(const QModelIndex &)),
            this, SLOT(newSelection(const QModelIndex &)));

    connect(m_dataModel, SIGNAL(dataDeleted(int)), this, SLOT(detachData(int)));
    connect(m_dataModel, SIGNAL(empty()), this, SLOT(closeView()));

    twGeoInfo->item(0,0)->setText(QString(lookupEnum(inPrimitiveType)));
    twGeoInfo->item(1,0)->setText(QString::number(m_dataModel->getNumInPrimitives()));
    twGeoInfo->item(0,2)->setText(QString(lookupEnum(outPrimitiveType)));
    twGeoInfo->item(1,2)->setText(QString::number(m_dataModel->getNumOutPrimitives()));

    if (GeoShaderDataModel::isBasicPrimitive(inPrimitiveType)) {
        twGeoInfo->hideColumn(1);
    } else {
        twGeoInfo->item(0,1)->setText(lookupEnum(GeoShaderDataModel::getBasePrimitive(inPrimitiveType)));
        twGeoInfo->item(1,1)->setText(QString::number(m_dataModel->getNumSubInPrimitives()));
    }

    twGeoInfo->resizeColumnsToContents();
    twGeoInfo->resizeRowsToContents();
    twGeoInfo->setSelectionBehavior(QAbstractItemView::SelectRows);

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

    m_dataSelection = DATA_CURRENT;

    m_maxScatterDataElements = MAX(m_dataModel->getNumInPrimitives(),
                                m_dataModel->getNumOutVertices());
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
    m_qGLscatter->setData(m_scatterPositions, m_scatterColorsAndSizes, 0);
    on_slPointSize_valueChanged(300);

    setupMappingUI();

    updateGUI();
}

WatchGeoDataTree::~WatchGeoDataTree()
{
    m_qGLscatter->setData(m_scatterPositions, m_scatterColorsAndSizes, 0);
    delete [] m_scatterPositions;
    delete [] m_scatterColorsAndSizes;
}

void WatchGeoDataTree::setupMappingUI()
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

bool WatchGeoDataTree::countsAllZero()
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

void WatchGeoDataTree::updateDataCurrent(float *data, int *count, int dataStride,
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

void WatchGeoDataTree::updateDataVertex(float *data, int *count, int dataStride,
                    VertexBox *srcData, Mapping *mapping,
                    RangeMapping *rangeMapping,
                    float min, float max)
{
    float minmax[2] = {min, fabs(max - min) > FLT_EPSILON ? max : 1.0f + min};
    float *dd = data;
    float *ds = srcData->getDataPointer();
    bool  *dm = srcData->getDataMapPointer();

    clearData(data, srcData->getNumVertices(), dataStride, 0.0f);

    *count = 0;
    for (int i = 0; i < srcData->getNumVertices(); i++) {
        if (dm && !*dm) {
        } else if (ds[1] > 0.0f) {
            *dd = getMappedValueF(*ds, mapping, rangeMapping, minmax);
            dd += dataStride;
            (*count)++;
        }
        ds += 2;
        if (dm) dm++;
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
        switch (m_dataSelection) { \
            case DATA_CURRENT: \
                updateDataCurrent(m_scatterData##VAL, &m_scatterDataCount##VAL, \
                        m_scatterDataStride##VAL, \
                        m_dataModel->getDataColumnCurrentData(m.index), &m, &rm, \
                        dsMin##VAL->value(), dsMax##VAL->value()); \
                break; \
            case DATA_VERTEX: \
                updateDataVertex(m_scatterData##VAL, &m_scatterDataCount##VAL, \
                        m_scatterDataStride##VAL, \
                        m_dataModel->getDataColumnVertexData(m.index), &m, &rm, \
                        dsMin##VAL->value(), dsMax##VAL->value()); \
                break; \
        } \
    }

void WatchGeoDataTree::attachData(VertexBox *currentData, VertexBox *vertexData,
                                QString name)
{
    if (!currentData || !vertexData) {
        return;
    }

    if (m_dataModel->addData(currentData, vertexData, name)) {
        addMappingOptions(m_dataModel->getDataColumnCount()-1);
        updateGUI();
    }
}

void WatchGeoDataTree::detachData(int idx)
{
    dbgPrint(DBGLVL_DEBUG, "WatchGeoDataTree::detachData: idx=%i\n", idx);
    delMappingOptions(idx);
}

void WatchGeoDataTree::updateGUI()
{
    int i;

    QString title("");

    for (i = 1; i < m_dataModel->columnCount(); i++) {
        title += m_dataModel->headerData(i, Qt::Horizontal).toString();
        if (i != m_dataModel->columnCount() - 1) {
            title += ", ";
        }
    }
    setWindowTitle(title);
}

void WatchGeoDataTree::updateView(bool force)
{
    UNUSED_ARG(force)
    /* TODO */
}

void WatchGeoDataTree::closeView()
{
    hide();
    deleteLater();
}

void WatchGeoDataTree::newSelection(const QModelIndex & index)
{
    if (index.isValid()) {
        int dataIdx = index.data(GeoShaderDataModel::IndexRole).toInt();
        if (dataIdx >= 0 && !index.data(GeoShaderDataModel::VertexRole).toBool()) {
            emit selectionChanged(dataIdx);
        }
    }
}

void WatchGeoDataTree::addMappingOptions(int idx)
{
    Mapping m;
    m.type = MAP_TYPE_VAR;
    m.index = idx;

    QString name = m_dataModel->getDataColumnName(idx);
    cbRed->addItem(name, QVariant(getIntFromMapping(m)));
    cbGreen->addItem(name, QVariant(getIntFromMapping(m)));
    cbBlue->addItem(name, QVariant(getIntFromMapping(m)));
    cbX->addItem(name, QVariant(getIntFromMapping(m)));
    cbY->addItem(name, QVariant(getIntFromMapping(m)));
    cbZ->addItem(name, QVariant(getIntFromMapping(m)));
}

void WatchGeoDataTree::delMappingOptions(int idx)
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
            tbMin##VAL->setText(QString::number(getDataMin(m.index))); \
            tbMax##VAL->setText(QString::number(getDataMax(m.index))); \
            break; \
        case RANGE_MAP_POSITIVE: \
            tbMin##VAL->setText(QString::number(MAX(getDataMin(m.index), 0.0))); \
            tbMax##VAL->setText(QString::number(MAX(getDataMax(m.index), 0.0))); \
            break; \
        case RANGE_MAP_NEGATIVE: \
            tbMin##VAL->setText(QString::number(MIN(getDataMin(m.index), 0.0))); \
            tbMax##VAL->setText(QString::number(MIN(getDataMax(m.index), 0.0))); \
            break; \
        case RANGE_MAP_ABSOLUTE: \
            tbMin##VAL->setText(QString::number(getDataAbsMin(m.index))); \
            tbMax##VAL->setText(QString::number(getDataAbsMax(m.index))); \
            break; \
    } \

#define ON_CB_ACTIVATED(VAL) \
void WatchGeoDataTree::on_cb##VAL##_activated(int newIdx) \
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
        switch (m_dataSelection) { \
            case DATA_CURRENT: \
                connect(m_dataModel->getDataColumnCurrentData(m.index), SIGNAL(dataChanged()), \
                        this, SLOT(mappingDataChanged##VAL())); \
                updateDataCurrent(m_scatterData##VAL, &m_scatterDataCount##VAL,\
                        m_scatterDataStride##VAL, \
                        m_dataModel->getDataColumnCurrentData(m.index), &m, &rm, \
                        dsMin##VAL->value(), dsMax##VAL->value()); \
                break; \
            case DATA_VERTEX: \
                connect(m_dataModel->getDataColumnVertexData(m.index), SIGNAL(dataChanged()), \
                        this, SLOT(mappingDataChanged##VAL())); \
                updateDataVertex(m_scatterData##VAL, &m_scatterDataCount##VAL, \
                        m_scatterDataStride##VAL, \
                        m_dataModel->getDataColumnVertexData(m.index), &m, &rm, \
                        dsMin##VAL->value(), dsMax##VAL->value()); \
                break; \
        } \
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
void WatchGeoDataTree::on_cbMap##VAL##_activated(int newIdx) \
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
void WatchGeoDataTree::on_tbMin##VAL##_clicked() \
{ \
    Mapping m = getMappingFromInt(cb##VAL->itemData(cb##VAL->currentIndex()).toInt()); \
    RangeMapping rm = getRangeMappingFromInt(cbMap##VAL->itemData(cbMap##VAL->currentIndex()).toInt()); \
    if (m.type == MAP_TYPE_VAR) { \
        switch (rm.range) { \
            case RANGE_MAP_DEFAULT: \
                dsMin##VAL->setValue(getDataMin(m.index)); \
                break; \
            case RANGE_MAP_POSITIVE: \
                dsMin##VAL->setValue(MAX(getDataMin(m.index), 0.0)); \
                break; \
            case RANGE_MAP_NEGATIVE: \
                dsMin##VAL->setValue(MIN(getDataMin(m.index), 0.0)); \
                break; \
            case RANGE_MAP_ABSOLUTE: \
                dsMin##VAL->setValue(getDataAbsMin(m.index)); \
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
void WatchGeoDataTree::on_tbMax##VAL##_clicked() \
{ \
    Mapping m = getMappingFromInt(cb##VAL->itemData(cb##VAL->currentIndex()).toInt()); \
    RangeMapping rm = getRangeMappingFromInt(cbMap##VAL->itemData(cbMap##VAL->currentIndex()).toInt()); \
    if (m.type == MAP_TYPE_VAR) { \
        switch (rm.range) { \
            case RANGE_MAP_DEFAULT: \
                dsMax##VAL->setValue(getDataMax(m.index)); \
                break; \
            case RANGE_MAP_POSITIVE: \
                dsMax##VAL->setValue(MAX(getDataMax(m.index), 0.0)); \
                break; \
            case RANGE_MAP_NEGATIVE: \
                dsMax##VAL->setValue(MIN(getDataMax(m.index), 0.0)); \
                break; \
            case RANGE_MAP_ABSOLUTE: \
                dsMax##VAL->setValue(getDataAbsMax(m.index)); \
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
void WatchGeoDataTree::on_dsMin##VAL##_valueChanged(double d)\
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
void WatchGeoDataTree::on_dsMax##VAL##_valueChanged(double d) \
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
void WatchGeoDataTree::on_tbSwitch##VAL##_clicked() \
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
void WatchGeoDataTree::mappingDataChanged##VAL() \
{ \
    VertexBox *sendingVB = static_cast<VertexBox*>(sender()); \
    Mapping m = getMappingFromInt(cb##VAL->itemData(cb##VAL->currentIndex()).toInt()); \
    if (sendingVB) { \
        VertexBox *activeVB = NULL; \
        switch (m_dataSelection) { \
            case DATA_CURRENT: \
                activeVB = m_dataModel->getDataColumnCurrentData(m.index); \
                break; \
            case DATA_VERTEX: \
                activeVB = m_dataModel->getDataColumnVertexData(m.index); \
                break; \
        } \
        if (activeVB != sendingVB) { \
            disconnect(sendingVB, SIGNAL(dataChanged()), this, SLOT(mappingDataChanged##VAL())); \
            return; \
        } \
    } \
    RangeMapping rm = getRangeMappingFromInt(cbMap##VAL->itemData(cbMap##VAL->currentIndex()).toInt()); \
    switch (rm.range) { \
        case RANGE_MAP_DEFAULT: \
            tbMin##VAL->setText(QString::number(getDataMin(m.index))); \
            tbMax##VAL->setText(QString::number(getDataMax(m.index))); \
            break; \
        case RANGE_MAP_POSITIVE: \
            tbMin##VAL->setText(QString::number(MAX(getDataMin(m.index), 0.0))); \
            tbMax##VAL->setText(QString::number(MAX(getDataMax(m.index), 0.0))); \
            break; \
        case RANGE_MAP_NEGATIVE: \
            tbMin##VAL->setText(QString::number(MIN(getDataMin(m.index), 0.0))); \
            tbMax##VAL->setText(QString::number(MIN(getDataMax(m.index), 0.0))); \
            break; \
        case RANGE_MAP_ABSOLUTE: \
            tbMin##VAL->setText(QString::number(getDataAbsMin(m.index))); \
            tbMax##VAL->setText(QString::number(getDataAbsMax(m.index))); \
            break; \
    } \
    switch (m_dataSelection) { \
        case DATA_CURRENT: \
            updateDataCurrent(m_scatterData##VAL, &m_scatterDataCount##VAL, m_scatterDataStride##VAL, \
                    sendingVB, &m, &rm, dsMin##VAL->value(), dsMax##VAL->value()); \
            break; \
        case DATA_VERTEX: \
            updateDataVertex(m_scatterData##VAL, &m_scatterDataCount##VAL, m_scatterDataStride##VAL, \
                    sendingVB, &m, &rm, dsMin##VAL->value(), dsMax##VAL->value()); \
            break; \
    } \
    m_qGLscatter->updateGL(); \
}

MAPPING_DATA_CHANGED(Red)
MAPPING_DATA_CHANGED(Green)
MAPPING_DATA_CHANGED(Blue)
MAPPING_DATA_CHANGED(X)
MAPPING_DATA_CHANGED(Y)
MAPPING_DATA_CHANGED(Z)

void WatchGeoDataTree::on_tbDataSelection_clicked()
{
    /* TODO: switch between display of current and emitted vertex values */
    /* change icon, etc. */
    if (m_dataSelection == DATA_VERTEX) {
        m_dataSelection = DATA_CURRENT;
    } else if (m_dataSelection == DATA_CURRENT) {
        m_dataSelection = DATA_VERTEX;
    }
    clearData(m_scatterDataX, m_maxScatterDataElements, 3, 0.0f);
    clearData(m_scatterDataY, m_maxScatterDataElements, 3, 0.0f);
    clearData(m_scatterDataZ, m_maxScatterDataElements, 3, 0.0f);
    clearData(m_scatterDataRed, m_maxScatterDataElements, 3, 0.0f);
    clearData(m_scatterDataGreen, m_maxScatterDataElements, 3, 0.0f);
    clearData(m_scatterDataBlue, m_maxScatterDataElements, 3, 0.0f);
    m_scatterDataCountX = 0;
    m_scatterDataCountY = 0;
    m_scatterDataCountZ = 0;
    m_scatterDataCountRed = 0;
    m_scatterDataCountGreen = 0;
    m_scatterDataCountBlue = 0;
    on_cbX_activated(cbX->currentIndex());
    on_cbY_activated(cbY->currentIndex());
    on_cbZ_activated(cbZ->currentIndex());
    on_cbRed_activated(cbRed->currentIndex());
    on_cbGreen_activated(cbGreen->currentIndex());
    on_cbBlue_activated(cbBlue->currentIndex());
    m_qGLscatter->updateGL();
    updateView(true);
}

float WatchGeoDataTree::getDataMin(int column)
{
    switch (m_dataSelection) {
        case DATA_CURRENT:
            return m_dataModel->getDataColumnCurrentData(column)->getMin();
        case DATA_VERTEX: {
            float min = FLT_MAX;
            VertexBox *vb = m_dataModel->getDataColumnVertexData(column);
            float *pData = vb->getDataPointer();
            bool *pDataMap = vb->getDataMapPointer();

            for (int i = 0; i < vb->getNumVertices(); i++) {
                if (*pDataMap && pData[1] > 0.0f && min > pData[0]) {
                    min = pData[0];
                }
                pData += 2;
                pDataMap += 1;
            }
            return min;
        }
    }
    return 0.0;
}

float WatchGeoDataTree::getDataMax(int column)
{
    switch (m_dataSelection) {
        case DATA_CURRENT:
            return m_dataModel->getDataColumnCurrentData(column)->getMax();
        case DATA_VERTEX: {
            float max = -FLT_MAX;
            VertexBox *vb = m_dataModel->getDataColumnVertexData(column);

            float *pData = vb->getDataPointer();
            bool *pDataMap = vb->getDataMapPointer();

            for (int i = 0; i < vb->getNumVertices(); i++) {
                if (*pDataMap && pData[1] > 0.0f && max < pData[0]) {
                    max = pData[0];
                }
                pData += 2;
                pDataMap += 1;
            }
            return max;
        }
    }
    return 0.0;
}

float WatchGeoDataTree::getDataAbsMin(int column)
{
    switch (m_dataSelection) {
        case DATA_CURRENT:
            return m_dataModel->getDataColumnCurrentData(column)->getAbsMin();
            break;
        case DATA_VERTEX: {
            float min = FLT_MAX;
            VertexBox *vb = m_dataModel->getDataColumnVertexData(column);
            float *pData = vb->getDataPointer();
            bool *pDataMap = vb->getDataMapPointer();

            for (int i = 0; i < vb->getNumVertices(); i++) {
                if (*pDataMap && pData[1] > 0.0f && min > fabs(pData[0])) {
                    min = fabs(pData[0]);
                }
                pData += 2;
                pDataMap += 1;
            }
            return min;
        }
    }
    return 0.0;
}

float WatchGeoDataTree::getDataAbsMax(int column)
{
    switch (m_dataSelection) {
        case DATA_CURRENT:
            return m_dataModel->getDataColumnCurrentData(column)->getAbsMax();
            break;
        case DATA_VERTEX: {
            float max = 0.0;
            VertexBox *vb = m_dataModel->getDataColumnVertexData(column);
            float *pData = vb->getDataPointer();
            bool *pDataMap = vb->getDataMapPointer();

            for (int i = 0; i < vb->getNumVertices(); i++) {
                if (*pDataMap && pData[1] > 0.0f && max < fabs(pData[0])) {
                    max = fabs(pData[0]);
                }
                pData += 2;
                pDataMap += 1;
            }
            return max;
        }
    }
    return 0.0;
}

void WatchGeoDataTree::on_slPointSize_valueChanged(int value)
{
    float newValue = 0.03*(float)value/(float)slPointSize->maximum();
    lbPointSize->setText(QString::number(newValue));
    m_qGLscatter->setPointSize(newValue);
    m_qGLscatter->updateGL();
}


