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

#include "watchVector.qt.h"
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QMessageBox>
#include <QtCore/QDir>
#include <QtWidgets/QFileDialog>
#include <QtGui/QImageWriter>

#include "mappings.h"

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

WatchVector::WatchVector(QWidget *parent) :
		WatchView(parent)
{
	int i;

	/* Setup GUI */
	setupUi(this);
	fMapping->setVisible(false);

	QGridLayout *gridLayout;
	gridLayout = new QGridLayout(fContent);
	gridLayout->setSpacing(0);
	gridLayout->setMargin(0);

	m_qScrollArea = new QScrollArea();
	m_qScrollArea->setBackgroundRole(QPalette::Dark);
	gridLayout->addWidget(m_qScrollArea);

	m_pImageView = new ImageView();
	m_qScrollArea->setWidget(m_pImageView);

	m_viewCenter[0] = -1;
	m_viewCenter[1] = -1;

	/* Initialize attachments */
	for (i = 0; i < MAX_ATTACHMENTS; i++) {
		m_pData[i] = NULL;
		m_qName[i] = QString("NULL");
	}

	/* Initialize color mapping comboboxes */
	Mapping m;
	m.index = 0;
	m.type = MAP_TYPE_BLACK;
	cbRed->addItem(QString("black"), QVariant(getIntFromMapping(m)));
	cbGreen->addItem(QString("black"), QVariant(getIntFromMapping(m)));
	cbBlue->addItem(QString("black"), QVariant(getIntFromMapping(m)));

	m.index = 1;
	m.type = MAP_TYPE_WHITE;
	cbRed->addItem(QString("red"), QVariant(getIntFromMapping(m)));
	cbGreen->addItem(QString("green"), QVariant(getIntFromMapping(m)));
	cbBlue->addItem(QString("blue"), QVariant(getIntFromMapping(m)));

	cbRed->setCurrentIndex(0);
	cbGreen->setCurrentIndex(0);
	cbBlue->setCurrentIndex(0);

	/* initialize range mapping comboboxes */
	RangeMapping rm;
	rm.index = 0;
	rm.range = RANGE_MAP_DEFAULT;
	cbMapRed->addItem(
			QIcon(QString::fromUtf8(":/icons/icons/watch-red-solid_32.png")),
			QString(""), QVariant(getIntFromRangeMapping(rm)));
	cbMapGreen->addItem(
			QIcon(QString::fromUtf8(":/icons/icons/watch-green-solid_32.png")),
			QString(""), QVariant(getIntFromRangeMapping(rm)));
	cbMapBlue->addItem(
			QIcon(QString::fromUtf8(":/icons/icons/watch-blue-solid_32.png")),
			QString(""), QVariant(getIntFromRangeMapping(rm)));
	rm.index = 1;
	rm.range = RANGE_MAP_POSITIVE;
	cbMapRed->addItem(
			QIcon(QString::fromUtf8(":/icons/icons/watch-red-positive_32.png")),
			QString(""), QVariant(getIntFromRangeMapping(rm)));
	cbMapGreen->addItem(
			QIcon(
					QString::fromUtf8(
							":/icons/icons/watch-green-positive_32.png")),
			QString(""), QVariant(getIntFromRangeMapping(rm)));
	cbMapBlue->addItem(
			QIcon(
					QString::fromUtf8(
							":/icons/icons/watch-blue-positive_32.png")),
			QString(""), QVariant(getIntFromRangeMapping(rm)));
	rm.index = 2;
	rm.range = RANGE_MAP_NEGATIVE;
	cbMapRed->addItem(
			QIcon(QString::fromUtf8(":/icons/icons/watch-red-negative_32.png")),
			QString(""), QVariant(getIntFromRangeMapping(rm)));
	cbMapGreen->addItem(
			QIcon(
					QString::fromUtf8(
							":/icons/icons/watch-green-negative_32.png")),
			QString(""), QVariant(getIntFromRangeMapping(rm)));
	cbMapBlue->addItem(
			QIcon(
					QString::fromUtf8(
							":/icons/icons/watch-blue-negative_32.png")),
			QString(""), QVariant(getIntFromRangeMapping(rm)));
	rm.index = 3;
	rm.range = RANGE_MAP_ABSOLUTE;
	cbMapRed->addItem(
			QIcon(QString::fromUtf8(":/icons/icons/watch-red-absolute_32.png")),
			QString(""), QVariant(getIntFromRangeMapping(rm)));
	cbMapGreen->addItem(
			QIcon(
					QString::fromUtf8(
							":/icons/icons/watch-green-absolute_32.png")),
			QString(""), QVariant(getIntFromRangeMapping(rm)));
	cbMapBlue->addItem(
			QIcon(
					QString::fromUtf8(
							":/icons/icons/watch-blue-absolute_32.png")),
			QString(""), QVariant(getIntFromRangeMapping(rm)));

	cbMapRed->setCurrentIndex(0);
	cbMapGreen->setCurrentIndex(0);
	cbMapBlue->setCurrentIndex(0);

	/* Initialize member variables */
	m_bNeedsUpdate = true;
	m_nActiveMappings = 0;

	updateGUI();

	connect(m_pImageView, SIGNAL(mousePosChanged(int, int)), this,
			SLOT(setMousePos(int, int)));
	connect(m_pImageView, SIGNAL(picked(int, int)), this,
			SLOT(newSelection(int, int)));
	connect(m_pImageView, SIGNAL(viewCenterChanged(int, int)), this,
			SLOT(newViewCenter(int, int)));

	m_pImageView->setFocusPolicy(Qt::StrongFocus);
	m_pImageView->setFocus();
}

void WatchVector::attachFpData(PixelBox *f, QString name)
{
	int idx;

	if (!f) {
		return;
	}

	if (!getNumFreeMappings()) {
		return;
	}

	if (getIndexFromPixelBox(f) != -1) {
		return;
	}

	/* Fill empty slot with attachment */
	idx = getFirstFreeMapping();
	m_pData[idx] = f;
	m_qName[idx] = name;
	m_nActiveMappings++;

	connect(f, SIGNAL(dataChanged()), this, SLOT(updateData()));
	connect(f, SIGNAL(dataDeleted()), this, SLOT(detachData()));
	connect(this->m_pImageView, SIGNAL(minMaxAreaChanged(const QRect&)), f,
			SLOT(setMinMaxArea(const QRect&)));
	connect(this->m_pImageView, SIGNAL(setMappingBounds()), this,
			SLOT(updateAllMinMax()));
	connect(f, SIGNAL(minMaxAreaChanged()), this, SLOT(onMinMaxAreaChanged()));

	addMappingOptions(idx);
	updateGUI();
}

void WatchVector::updateGUI()
{
	int i;
	Mapping m[3];
	RangeMapping rm[3];

	m[0] = getMappingFromInt(cbRed->itemData(cbRed->currentIndex()).toInt());
	m[1] = getMappingFromInt(
			cbGreen->itemData(cbGreen->currentIndex()).toInt());
	m[2] = getMappingFromInt(cbBlue->itemData(cbBlue->currentIndex()).toInt());
	rm[0] = getRangeMappingFromInt(
			cbMapRed->itemData(cbMapRed->currentIndex()).toInt());
	rm[1] = getRangeMappingFromInt(
			cbMapGreen->itemData(cbMapGreen->currentIndex()).toInt());
	rm[2] = getRangeMappingFromInt(
			cbMapBlue->itemData(cbMapBlue->currentIndex()).toInt());

	QString title;

	for (i = 0; i < 3; i++) {
		switch (m[i].type) {
		case MAP_TYPE_BLACK:
		case MAP_TYPE_OFF:
			title += QString("black");
			if (i == 0) {
				tbMinRed->setEnabled(false);
				tbMaxRed->setEnabled(false);
				dsMinRed->setEnabled(false);
				dsMaxRed->setEnabled(false);
				cbSyncRed->setEnabled(false);
				cbMapRed->setEnabled(false);
			} else if (i == 1) {
				tbMinGreen->setEnabled(false);
				tbMaxGreen->setEnabled(false);
				dsMinGreen->setEnabled(false);
				dsMaxGreen->setEnabled(false);
				cbSyncGreen->setEnabled(false);
				cbMapGreen->setEnabled(false);
			} else {
				tbMinBlue->setEnabled(false);
				tbMaxBlue->setEnabled(false);
				dsMinBlue->setEnabled(false);
				dsMaxBlue->setEnabled(false);
				cbSyncBlue->setEnabled(false);
				cbMapBlue->setEnabled(false);
			}
			break;
		case MAP_TYPE_WHITE:
			if (i == 0) {
				title += QString("red");
				tbMinRed->setEnabled(false);
				tbMaxRed->setEnabled(false);
				dsMinRed->setEnabled(false);
				dsMaxRed->setEnabled(false);
				cbSyncRed->setEnabled(false);
				cbMapRed->setEnabled(false);
			} else if (i == 1) {
				title += QString("green");
				tbMinGreen->setEnabled(false);
				tbMaxGreen->setEnabled(false);
				dsMinGreen->setEnabled(false);
				dsMaxGreen->setEnabled(false);
				cbSyncGreen->setEnabled(false);
				cbMapGreen->setEnabled(false);
			} else {
				title += QString("blue");
				tbMinBlue->setEnabled(false);
				tbMaxBlue->setEnabled(false);
				dsMinBlue->setEnabled(false);
				dsMaxBlue->setEnabled(false);
				cbSyncBlue->setEnabled(false);
				cbMapBlue->setEnabled(false);
			}
			break;
		case MAP_TYPE_VAR:
			switch (rm[i].range) {
			case RANGE_MAP_DEFAULT:
				if (i == 0) {
					title += cbRed->itemText(cbRed->currentIndex());
				} else if (i == 1) {
					title += cbGreen->itemText(cbGreen->currentIndex());
				} else {
					title += cbBlue->itemText(cbBlue->currentIndex());
				}
				break;
			case RANGE_MAP_POSITIVE:
				title += QString("+(");
				if (i == 0) {
					title += cbRed->itemText(cbRed->currentIndex());
				} else if (i == 1) {
					title += cbGreen->itemText(cbGreen->currentIndex());
				} else {
					title += cbBlue->itemText(cbBlue->currentIndex());
				}
				title += QString(")");
				break;
			case RANGE_MAP_NEGATIVE:
				title += QString("-(");
				if (i == 0) {
					title += cbRed->itemText(cbRed->currentIndex());
				} else if (i == 1) {
					title += cbGreen->itemText(cbGreen->currentIndex());
				} else {
					title += cbBlue->itemText(cbBlue->currentIndex());
				}
				title += QString(")");
				break;
			case RANGE_MAP_ABSOLUTE:
				title += QString("|");
				if (i == 0) {
					title += cbRed->itemText(cbRed->currentIndex());
				} else if (i == 1) {
					title += cbGreen->itemText(cbGreen->currentIndex());
				} else {
					title += cbBlue->itemText(cbBlue->currentIndex());
				}
				title += QString("|");
				break;
			}
			if (i == 0) {
				tbMinRed->setEnabled(true);
				tbMaxRed->setEnabled(true);
				dsMinRed->setEnabled(true);
				dsMaxRed->setEnabled(true);
				cbSyncRed->setEnabled(true);
				cbMapRed->setEnabled(true);
				switch (rm[0].range) {
				case RANGE_MAP_DEFAULT:
					tbMinRed->setText(
							QString::number(m_pData[m[0].index]->getMin()));
					tbMaxRed->setText(
							QString::number(m_pData[m[0].index]->getMax()));
					break;
				case RANGE_MAP_POSITIVE:
					tbMinRed->setText(
							QString::number(MAX(m_pData[m[0].index]->getMin(),
									0.0)));
					tbMaxRed->setText(
							QString::number(MAX(m_pData[m[0].index]->getMax(),
									0.0)));
					break;
				case RANGE_MAP_NEGATIVE:
					tbMinRed->setText(
							QString::number(MIN(m_pData[m[0].index]->getMin(),
									0.0)));
					tbMaxRed->setText(
							QString::number(MIN(m_pData[m[0].index]->getMax(),
									0.0)));
					break;
				case RANGE_MAP_ABSOLUTE:
					tbMinRed->setText(
							QString::number(m_pData[m[0].index]->getAbsMin()));
					tbMaxRed->setText(
							QString::number(m_pData[m[0].index]->getAbsMax()));
					break;
				}
			} else if (i == 1) {
				tbMinGreen->setEnabled(true);
				tbMaxGreen->setEnabled(true);
				dsMinGreen->setEnabled(true);
				dsMaxGreen->setEnabled(true);
				cbSyncGreen->setEnabled(true);
				cbMapGreen->setEnabled(true);
				switch (rm[1].range) {
				case RANGE_MAP_DEFAULT:
					tbMinGreen->setText(
							QString::number(m_pData[m[1].index]->getMin()));
					tbMaxGreen->setText(
							QString::number(m_pData[m[1].index]->getMax()));
					break;
				case RANGE_MAP_POSITIVE:
					tbMinGreen->setText(
							QString::number(MAX(m_pData[m[1].index]->getMin(),
									0.0)));
					tbMaxGreen->setText(
							QString::number(MAX(m_pData[m[1].index]->getMax(),
									0.0)));
					break;
				case RANGE_MAP_NEGATIVE:
					tbMinGreen->setText(
							QString::number(MIN(m_pData[m[1].index]->getMin(),
									0.0)));
					tbMaxGreen->setText(
							QString::number(MIN(m_pData[m[1].index]->getMax(),
									0.0)));
					break;
				case RANGE_MAP_ABSOLUTE:
					tbMinGreen->setText(
							QString::number(m_pData[m[1].index]->getAbsMin()));
					tbMaxGreen->setText(
							QString::number(m_pData[m[1].index]->getAbsMax()));
					break;
				}
			} else {
				tbMinBlue->setEnabled(true);
				tbMaxBlue->setEnabled(true);
				dsMinBlue->setEnabled(true);
				dsMaxBlue->setEnabled(true);
				cbSyncBlue->setEnabled(true);
				cbMapBlue->setEnabled(true);
				switch (rm[2].range) {
				case RANGE_MAP_DEFAULT:
					tbMinBlue->setText(
							QString::number(m_pData[m[2].index]->getMin()));
					tbMaxBlue->setText(
							QString::number(m_pData[m[2].index]->getMax()));
					break;
				case RANGE_MAP_POSITIVE:
					tbMinBlue->setText(
							QString::number(MAX(m_pData[m[2].index]->getMin(),
									0.0)));
					tbMaxBlue->setText(
							QString::number(MAX(m_pData[m[2].index]->getMax(),
									0.0)));
					break;
				case RANGE_MAP_NEGATIVE:
					tbMinBlue->setText(
							QString::number(MIN(m_pData[m[2].index]->getMin(),
									0.0)));
					tbMaxBlue->setText(
							QString::number(MIN(m_pData[m[2].index]->getMax(),
									0.0)));
					break;
				case RANGE_MAP_ABSOLUTE:
					tbMinBlue->setText(
							QString::number(m_pData[m[2].index]->getAbsMin()));
					tbMaxBlue->setText(
							QString::number(m_pData[m[2].index]->getAbsMax()));
					break;
				}
			}
			break;
		}
		if (i < 2) {
			title += QString(", ");
		}
	}

	setWindowTitle(title);
}

int WatchVector::getNumFreeMappings()
{
	int i = MAX_ATTACHMENTS - m_nActiveMappings;

	return (i > 0) ? i : 0;
}

int WatchVector::getFirstFreeMapping()
{
	int i;

	for (i = 0; i < MAX_ATTACHMENTS; i++) {
		if (m_pData[i] == NULL) {
			return i;
		}
	}

	return -1;
}

void WatchVector::addMappingOptions(int idx)
{
	Mapping m;
	m.type = MAP_TYPE_VAR;
	m.index = idx;

	cbRed->addItem(m_qName[idx], QVariant(getIntFromMapping(m)));
	cbGreen->addItem(m_qName[idx], QVariant(getIntFromMapping(m)));
	cbBlue->addItem(m_qName[idx], QVariant(getIntFromMapping(m)));

	/* Check if this new mapping could be used */
	int itemIndex;
	QVariant itemData;
	Mapping itemMapping;

	/* Red */
	itemIndex = cbRed->currentIndex();
	itemData = cbRed->itemData(itemIndex);
	itemMapping = getMappingFromInt(itemData.toInt());

	if (itemMapping.type == MAP_TYPE_BLACK) {
		itemMapping.type = MAP_TYPE_VAR;
		itemMapping.index = idx;
		itemIndex = cbRed->findData(QVariant(getIntFromMapping(itemMapping)));
		cbRed->setCurrentIndex(itemIndex);
		on_tbMinRed_clicked();
		on_tbMaxRed_clicked();
		return;
	}

	/* Green */
	itemIndex = cbGreen->currentIndex();
	itemData = cbGreen->itemData(itemIndex);
	itemMapping = getMappingFromInt(itemData.toInt());

	if (itemMapping.type == MAP_TYPE_BLACK) {
		itemMapping.type = MAP_TYPE_VAR;
		itemMapping.index = idx;
		itemIndex = cbGreen->findData(QVariant(getIntFromMapping(itemMapping)));
		cbGreen->setCurrentIndex(itemIndex);
		on_tbMinGreen_clicked();
		on_tbMaxGreen_clicked();
		return;
	}

	/* Blue */
	itemIndex = cbBlue->currentIndex();
	itemData = cbBlue->itemData(itemIndex);
	itemMapping = getMappingFromInt(itemData.toInt());

	if (itemMapping.type == MAP_TYPE_BLACK) {
		itemMapping.type = MAP_TYPE_VAR;
		itemMapping.index = idx;
		itemIndex = cbBlue->findData(QVariant(getIntFromMapping(itemMapping)));
		cbBlue->setCurrentIndex(itemIndex);
		on_tbMinBlue_clicked();
		on_tbMaxBlue_clicked();
		return;
	}
}

void WatchVector::delMappingOptions(int idx)
{
	/* Check if it's in use right now */
	QVariant data;
	Mapping m;

	data = cbRed->itemData(cbRed->currentIndex());
	m = getMappingFromInt(data.toInt());
	if (m.index == idx) {
		cbRed->setCurrentIndex(0);
	}
	data = cbGreen->itemData(cbGreen->currentIndex());
	m = getMappingFromInt(data.toInt());
	if (m.index == idx) {
		cbGreen->setCurrentIndex(0);
	}
	data = cbBlue->itemData(cbBlue->currentIndex());
	m = getMappingFromInt(data.toInt());
	if (m.index == idx) {
		cbBlue->setCurrentIndex(0);
	}

	/* Delete options in comboboxes */
	int map;
	m.index = idx;

	m.type = MAP_TYPE_VAR;
	map = getIntFromMapping(m);
	idx = cbRed->findData(QVariant(map));
	cbRed->removeItem(idx);
	cbGreen->removeItem(idx);
	cbBlue->removeItem(idx);
}

void WatchVector::on_cbRed_activated(int newIdx)
{
	UNUSED_ARG(newIdx)
	updateView(true);
}

void WatchVector::on_cbGreen_activated(int newIdx)
{
	UNUSED_ARG(newIdx)
	updateView(true);
}

void WatchVector::on_cbBlue_activated(int newIdx)
{
	UNUSED_ARG(newIdx)
	updateView(true);
}

void WatchVector::on_cbMapRed_activated(int newIdx)
{
	UNUSED_ARG(newIdx)
	updateView(true);
}

void WatchVector::on_cbMapGreen_activated(int newIdx)
{
	UNUSED_ARG(newIdx)
	updateView(true);
}

void WatchVector::on_cbMapBlue_activated(int newIdx)
{
	UNUSED_ARG(newIdx)
	updateView(true);
}

void WatchVector::on_tbMinRed_clicked()
{
	Mapping m = getMappingFromInt(
			cbRed->itemData(cbRed->currentIndex()).toInt());
	RangeMapping rm = getRangeMappingFromInt(
			cbMapRed->itemData(cbMapRed->currentIndex()).toInt());

	if (m.type == MAP_TYPE_VAR) {
		switch (rm.range) {
		case RANGE_MAP_DEFAULT:
			dsMinRed->setValue(m_pData[m.index]->getMin());
			break;
		case RANGE_MAP_POSITIVE:
			dsMinRed->setValue(MAX(m_pData[m.index]->getMin(), 0.0));
			break;
		case RANGE_MAP_NEGATIVE:
			dsMinRed->setValue(MIN(m_pData[m.index]->getMin(), 0.0));
			break;
		case RANGE_MAP_ABSOLUTE:
			dsMinRed->setValue(m_pData[m.index]->getAbsMin());
			break;
		}
	}
	updateView(true);
}

void WatchVector::on_tbMaxRed_clicked()
{
	Mapping m = getMappingFromInt(
			cbRed->itemData(cbRed->currentIndex()).toInt());
	RangeMapping rm = getRangeMappingFromInt(
			cbMapRed->itemData(cbMapRed->currentIndex()).toInt());

	if (m.type == MAP_TYPE_VAR) {
		switch (rm.range) {
		case RANGE_MAP_DEFAULT:
			dsMaxRed->setValue(m_pData[m.index]->getMax());
			break;
		case RANGE_MAP_POSITIVE:
			dsMaxRed->setValue(MAX(m_pData[m.index]->getMax(), 0.0));
			break;
		case RANGE_MAP_NEGATIVE:
			dsMaxRed->setValue(MIN(m_pData[m.index]->getMax(), 0.0));
			break;
		case RANGE_MAP_ABSOLUTE:
			dsMaxRed->setValue(m_pData[m.index]->getAbsMax());
			break;
		}
	}
}

void WatchVector::on_tbMinGreen_clicked()
{
	Mapping m = getMappingFromInt(
			cbGreen->itemData(cbGreen->currentIndex()).toInt());
	RangeMapping rm = getRangeMappingFromInt(
			cbMapGreen->itemData(cbMapGreen->currentIndex()).toInt());

	if (m.type == MAP_TYPE_VAR) {
		switch (rm.range) {
		case RANGE_MAP_DEFAULT:
			dsMinGreen->setValue(m_pData[m.index]->getMin());
			break;
		case RANGE_MAP_POSITIVE:
			dsMinGreen->setValue(MAX(m_pData[m.index]->getMin(), 0.0));
			break;
		case RANGE_MAP_NEGATIVE:
			dsMinGreen->setValue(MIN(m_pData[m.index]->getMin(), 0.0));
			break;
		case RANGE_MAP_ABSOLUTE:
			dsMinGreen->setValue(m_pData[m.index]->getAbsMin());
			break;
		}
	}
}

void WatchVector::on_tbMaxGreen_clicked()
{
	Mapping m = getMappingFromInt(
			cbGreen->itemData(cbGreen->currentIndex()).toInt());
	RangeMapping rm = getRangeMappingFromInt(
			cbMapGreen->itemData(cbMapGreen->currentIndex()).toInt());

	if (m.type == MAP_TYPE_VAR) {
		switch (rm.range) {
		case RANGE_MAP_DEFAULT:
			dsMaxGreen->setValue(m_pData[m.index]->getMax());
			break;
		case RANGE_MAP_POSITIVE:
			dsMaxGreen->setValue(MAX(m_pData[m.index]->getMax(), 0.0));
			break;
		case RANGE_MAP_NEGATIVE:
			dsMaxGreen->setValue(MIN(m_pData[m.index]->getMax(), 0.0));
			break;
		case RANGE_MAP_ABSOLUTE:
			dsMaxGreen->setValue(m_pData[m.index]->getAbsMax());
			break;
		}
	}
}

void WatchVector::on_tbMinBlue_clicked()
{
	Mapping m = getMappingFromInt(
			cbBlue->itemData(cbBlue->currentIndex()).toInt());
	RangeMapping rm = getRangeMappingFromInt(
			cbMapBlue->itemData(cbMapBlue->currentIndex()).toInt());

	if (m.type == MAP_TYPE_VAR) {
		switch (rm.range) {
		case RANGE_MAP_DEFAULT:
			dsMinBlue->setValue(m_pData[m.index]->getMin());
			break;
		case RANGE_MAP_POSITIVE:
			dsMinBlue->setValue(MAX(m_pData[m.index]->getMin(), 0.0));
			break;
		case RANGE_MAP_NEGATIVE:
			dsMinBlue->setValue(MIN(m_pData[m.index]->getMin(), 0.0));
			break;
		case RANGE_MAP_ABSOLUTE:
			dsMinBlue->setValue(m_pData[m.index]->getAbsMin());
			break;
		}
	}
}

void WatchVector::on_tbMaxBlue_clicked()
{
	Mapping m = getMappingFromInt(
			cbBlue->itemData(cbBlue->currentIndex()).toInt());
	RangeMapping rm = getRangeMappingFromInt(
			cbMapBlue->itemData(cbMapBlue->currentIndex()).toInt());

	if (m.type == MAP_TYPE_VAR) {
		switch (rm.range) {
		case RANGE_MAP_DEFAULT:
			dsMaxBlue->setValue(m_pData[m.index]->getMax());
			break;
		case RANGE_MAP_POSITIVE:
			dsMaxBlue->setValue(MAX(m_pData[m.index]->getMax(), 0.0));
			break;
		case RANGE_MAP_NEGATIVE:
			dsMaxBlue->setValue(MIN(m_pData[m.index]->getMax(), 0.0));
			break;
		case RANGE_MAP_ABSOLUTE:
			dsMaxBlue->setValue(m_pData[m.index]->getAbsMax());
			break;
		}
	}
}

void WatchVector::on_dsMinRed_valueChanged(double d)
{
	UNUSED_ARG(d)
	updateView(true);
}

void WatchVector::on_dsMaxRed_valueChanged(double d)
{
	UNUSED_ARG(d)
	updateView(true);
}

void WatchVector::on_dsMinGreen_valueChanged(double d)
{
	UNUSED_ARG(d)
	updateView(true);
}

void WatchVector::on_dsMaxGreen_valueChanged(double d)
{
	UNUSED_ARG(d)
	updateView(true);
}

void WatchVector::on_dsMinBlue_valueChanged(double d)
{
	UNUSED_ARG(d)
	updateView(true);
}

void WatchVector::on_dsMaxBlue_valueChanged(double d)
{
	UNUSED_ARG(d)
	updateView(true);
}

void WatchVector::on_tbSwitchRed_clicked()
{
	float tmp = dsMinRed->value();
	dsMinRed->setValue(dsMaxRed->value());
	dsMaxRed->setValue(tmp);
	updateView(true);
}

void WatchVector::on_tbSwitchGreen_clicked()
{
	float tmp = dsMinGreen->value();
	dsMinGreen->setValue(dsMaxGreen->value());
	dsMaxGreen->setValue(tmp);
	updateView(true);
}

void WatchVector::on_tbSwitchBlue_clicked()
{
	float tmp = dsMinBlue->value();
	dsMinBlue->setValue(dsMaxBlue->value());
	dsMaxBlue->setValue(tmp);
	updateView(true);
}

void WatchVector::on_tbSaveImage_clicked()
{
	static QStringList history;
	static QDir directory = QDir::current();

	if (m_pImageView) {
		QFileDialog *sDialog = new QFileDialog(this, QString("Save image"));

		sDialog->setAcceptMode(QFileDialog::AcceptSave);
		sDialog->setFileMode(QFileDialog::AnyFile);
		QStringList formatDesc;
		formatDesc << "Portable Network Graphics (*.png)"
				<< "Windows Bitmap (*.bmp)"
				<< "Joint Photographic Experts Group (*.jpg, *.jepg)"
				<< "Portable Pixmap (*.ppm)"
				<< "Tagged Image File Format (*.tif, *.tiff)"
				<< "X11 Bitmap (*.xbm, *.xpm)";
		sDialog->setNameFilters(formatDesc);

		if (!(history.isEmpty())) {
			sDialog->setHistory(history);
		}

		sDialog->setDirectory(directory);

		if (sDialog->exec()) {
			QStringList files = sDialog->selectedFiles();

			if (!files.isEmpty()) {
				QString selected = files[0];
				QFileInfo fileInfo(selected);

				QImage *img;
				img = drawNewImage(false);

				if (!(img->save(selected))) {

					QString forceFilter;
					QString filter = sDialog->selectedNameFilter();
					if (filter
							== QString("Portable Network Graphics (*.png)")) {
						forceFilter.append("png");
					} else if (filter == QString("Windows Bitmap (*.bmp)")) {
						forceFilter.append("bmp");
					} else if (filter
							== QString(
									"Joint Photographic Experts Group (*.jpg, *.jepg)")) {
						forceFilter.append("jpg");
					} else if (filter == QString("Portable Pixmap (*.ppm)")) {
						forceFilter.append("ppm");
					} else if (filter
							== QString(
									"Tagged Image File Format (*.tif, *.tiff)")) {
						forceFilter.append("tif");
					} else if (filter == QString("X11 Bitmap (*.xbm, *.xpm)")) {
						forceFilter.append("xbm");
					}

					img->save(selected, forceFilter.toLatin1().data());
				}
				delete img;
			}
		}

		history = sDialog->history();
		directory = sDialog->directory();

		delete sDialog;
	}
}

static void setImageRedChannel(QImage *image, bool *pCover, int value)
{
	for (int y = 0; y < image->height(); y++) {
		for (int x = 0; x < image->width(); x++) {
			QColor c(image->pixel(x, y));
			if (pCover[y * image->width() + x]) {
				c.setRed(value);
			} else {
				c.setRed(((x / 8) % 2) == ((y / 8) % 2) ? 255 : 204);
			}
			image->setPixel(x, y, c.rgb());
		}
	}
}
static void setImageGreenChannel(QImage *image, bool *pCover, int value)
{
	for (int y = 0; y < image->height(); y++) {
		for (int x = 0; x < image->width(); x++) {
			QColor c(image->pixel(x, y));
			if (pCover[y * image->width() + x]) {
				c.setGreen(value);
			} else {
				c.setGreen(((x / 8) % 2) == ((y / 8) % 2) ? 255 : 204);
			}
			image->setPixel(x, y, c.rgb());
		}
	}
}

static void setImageBlueChannel(QImage *image, bool *pCover, int value)
{
	for (int y = 0; y < image->height(); y++) {
		for (int x = 0; x < image->width(); x++) {
			QColor c(image->pixel(x, y));
			if (pCover[y * image->width() + x]) {
				c.setBlue(value);
			} else {
				c.setBlue(((x / 8) % 2) == ((y / 8) % 2) ? 255 : 204);
			}
			image->setPixel(x, y, c.rgb());
		}
	}
}

QImage* WatchVector::drawNewImage(bool useAlpha)
{
	Mapping mappings[3];
	RangeMapping rangemappings[3];
	float minmax[3][2];
	int width, height;
	bool *pCover = NULL;
	//bool  *pValid[3] = {NULL, NULL, NULL};
	//float *pData[3] = {NULL, NULL, NULL};

	/* Search for covermap */
	for (int i = 0; i < MAX_ATTACHMENTS; i++) {
		if (m_pData[i]) {
			pCover = m_pData[i]->getCoveragePointer();
		}
	}

	/* Check if attached data is valid */
	width = 0;
	height = 0;
	for (int i = 0; i < MAX_ATTACHMENTS; i++) {
		if (m_pData[i]) {
			if (width == 0) {
				width = m_pData[i]->getWidth();
			} else {
				if (width != m_pData[i]->getWidth()) {
					QMessageBox::critical(this, "Internal Error",
							"WatchVector is composed of differently sized float "
									"boxes.<BR>Please report this probem to "
									"<A HREF=\"mailto:glsldevil@vis.uni-stuttgart.de\">"
									"glsldevil@vis.uni-stuttgart.de</A>.",
							QMessageBox::Ok);
				}
			}
			if (height == 0) {
				height = m_pData[i]->getHeight();
			} else {
				if (height != m_pData[i]->getHeight()) {
					QMessageBox::critical(this, "Internal Error",
							"WatchVector is composed of differently sized float "
									"boxes.<BR>Please report this probem to "
									"<A HREF=\"mailto:glsldevil@vis.uni-stuttgart.de\">"
									"glsldevil@vis.uni-stuttgart.de</A>.",
							QMessageBox::Ok);
				}
			}
		}
	}

	/* Apply mapping */
	mappings[0] = getMappingFromInt(
			cbRed->itemData(cbRed->currentIndex()).toInt());
	mappings[1] = getMappingFromInt(
			cbGreen->itemData(cbGreen->currentIndex()).toInt());
	mappings[2] = getMappingFromInt(
			cbBlue->itemData(cbBlue->currentIndex()).toInt());

	rangemappings[0] = getRangeMappingFromInt(
			cbMapRed->itemData(cbMapRed->currentIndex()).toInt());
	rangemappings[1] = getRangeMappingFromInt(
			cbMapGreen->itemData(cbMapGreen->currentIndex()).toInt());
	rangemappings[2] = getRangeMappingFromInt(
			cbMapBlue->itemData(cbMapBlue->currentIndex()).toInt());

	minmax[0][0] = dsMinRed->value();
	minmax[0][1] =
			fabs(dsMaxRed->value() - minmax[0][0]) > FLT_EPSILON ?
					dsMaxRed->value() : 1.0f + minmax[0][0];
	minmax[1][0] = dsMinGreen->value();
	minmax[1][1] =
			fabs(dsMaxGreen->value() - minmax[1][0]) > FLT_EPSILON ?
					dsMaxGreen->value() : 1.0f + minmax[1][0];
	minmax[2][0] = dsMinBlue->value();
	minmax[2][1] =
			fabs(dsMaxBlue->value() - minmax[2][0]) > FLT_EPSILON ?
					dsMaxBlue->value() : 1.0f + minmax[2][0];

	//for(int i = 0; i < 3; i++) {
	//    if (mappings[i].type != MAP_TYPE_BLACK &&
	//            mappings[i].type != MAP_TYPE_WHITE) {
	//        pData[i] = m_pData[mappings[i].index]->getDataPointer();
	//        pValid[i] = m_pData[mappings[i].index]->getDataMapPointer();
	//    }
	//}

	QImage *image = new QImage(width, height,
			useAlpha ? QImage::Format_ARGB32 : QImage::Format_RGB32);

#if 1
	if (mappings[0].type == MAP_TYPE_VAR && m_pData[mappings[0].index]) {
		m_pData[mappings[0].index]->setByteImageRedChannel(image, &mappings[0],
				&rangemappings[0], minmax[0], useAlpha);
	} else if (mappings[0].type == MAP_TYPE_WHITE) {
		setImageRedChannel(image, pCover, 255);
	} else {
		setImageRedChannel(image, pCover, 0);
	}
	if (mappings[1].type == MAP_TYPE_VAR && m_pData[mappings[1].index]) {
		m_pData[mappings[1].index]->setByteImageGreenChannel(image,
				&mappings[1], &rangemappings[1], minmax[1], useAlpha);
	} else if (mappings[1].type == MAP_TYPE_WHITE) {
		setImageGreenChannel(image, pCover, 255);
	} else {
		setImageGreenChannel(image, pCover, 0);
	}
	if (mappings[2].type == MAP_TYPE_VAR && m_pData[mappings[2].index]) {
		m_pData[mappings[2].index]->setByteImageBlueChannel(image, &mappings[2],
				&rangemappings[2], minmax[2], useAlpha);
	} else if (mappings[2].type == MAP_TYPE_WHITE) {
		setImageBlueChannel(image, pCover, 255);
	} else {
		setImageBlueChannel(image, pCover, 0);
	}
#else
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (*pCover && ((pData[0] && *pValid[0]) ||
							(pData[1] && *pValid[1]) ||
							(pData[2] && *pValid[2]) ||
							mappings[0].type != MAP_TYPE_VAR ||
							mappings[1].type != MAP_TYPE_VAR ||
							mappings[2].type != MAP_TYPE_VAR)) {
				QColor color;
				if (pData[0] && *pValid[0]++ && mappings[0].type == MAP_TYPE_VAR) {
					color.setRed(getMappedValueI(*pData[0]++, &mappings[0],
									&rangemappings[0], minmax[0]));
				} else if (mappings[0].type == MAP_TYPE_WHITE) {
					color.setRed(255);
				} else {
					color.setRed(0);
				}
				if (pData[1] && *pValid[1]++ && mappings[1].type == MAP_TYPE_VAR) {
					color.setGreen(getMappedValueI(*pData[1]++, &mappings[1],
									&rangemappings[1], minmax[1]));
				} else if (mappings[1].type == MAP_TYPE_WHITE) {
					color.setGreen(255);
				} else {
					color.setGreen(0);
				}
				if (pData[2] && *pValid[2]++ && mappings[2].type == MAP_TYPE_VAR) {
					color.setBlue(getMappedValueI(*pData[2]++, &mappings[2],
									&rangemappings[2], minmax[2]));
				} else if (mappings[2].type == MAP_TYPE_WHITE) {
					color.setBlue(255);
				} else {
					color.setBlue(0);
				}
				if (useAlpha) {
					image->setPixel(x, y, color.rgb());
				} else {
					image->setPixel(x, y, color.rgb());
				}
			} else {
				if (useAlpha) {
					image->setPixel(x, y, QColor(0, 0, 0).rgb());
				} else {
					if ( ((x/8)%2) == ((y/8)%2)) {
						image->setPixel(x, y, QColor(255,255,255).rgb());
					} else {
						image->setPixel(x, y, QColor(204,204,204).rgb());
					}
					for (i = 0; i < 3; i++) {
						if (pData[i]) {
							pData[i]++;
							pValid[i]++;
						}
					}
				}
			}
			pCover++;
		}
	}
#endif
	return image;
}

void WatchVector::updateView(bool force)
{
	if (m_bNeedsUpdate || force) {
		/* update GUI, min/max may have changed */
		updateGUI();

		QImage *image = drawNewImage(false);

		int scrollPosX = m_qScrollArea->horizontalScrollBar()->value();
		int scrollPosY = m_qScrollArea->verticalScrollBar()->value();

		setUpdatesEnabled(false);
		m_pImageView->resize(image->width(), image->height());
		m_pImageView->setImage(*image);
		m_qScrollArea->horizontalScrollBar()->setValue(scrollPosX);
		m_qScrollArea->verticalScrollBar()->setValue(scrollPosY);
		setUpdatesEnabled(true);
		update();

		delete image;
	}
	m_bNeedsUpdate = false;
}

void WatchVector::updateData()
{
	m_bNeedsUpdate = true;
}

int WatchVector::getIndexFromPixelBox(PixelBox *f)
{
	int i;

	for (i = 0; i < MAX_ATTACHMENTS; i++) {
		if (m_pData[i] == f) {
			return i;
		}
	}

	return -1;
}

void WatchVector::detachData()
{
	PixelBox *f = static_cast<PixelBox*>(sender());
	int idx = getIndexFromPixelBox(f);

	if (idx != -1) {
		m_nActiveMappings--;
		m_pData[idx] = NULL;
		m_qName[idx] = QString("NULL");
		m_bNeedsUpdate = true;
		delMappingOptions(idx);
	}

	if (m_nActiveMappings == 0) {
		closeView();
	}
}

void WatchVector::closeView()
{
	hide();
	deleteLater();
}

void WatchVector::setMousePos(int x, int y)
{
	if (x >= 0 && y >= 0) {
		PixelBox *activeChannels[3];
		QVariant values[3];
		bool active[3];
		getActiveChannels(activeChannels);
		for (int i = 0; i < 3; i++) {
			if (activeChannels[i]) {
				active[i] = activeChannels[i]->getDataValue(x, y, &values[i]);
			} else {
				active[i] = false;
				values[i] = 0.0;
			}
		}
		emit mouseOverValuesChanged(x, y, active, values);
	} else {
		emit mouseOverValuesChanged(-1, -1, NULL, NULL);
	}
}

void WatchVector::newSelection(int x, int y)
{
	emit selectionChanged(x, y);
}

void WatchVector::setZoomMode()
{
	m_pImageView->setMouseMode(ImageView::MM_ZOOM);
}

void WatchVector::setPickMode()
{
	m_pImageView->setMouseMode(ImageView::MM_PICK);
}

void WatchVector::setMinMaxMode()
{
	m_pImageView->setMouseMode(ImageView::MM_MINMAX);
}

void WatchVector::updateAllMinMax()
{
	on_tbMinRed_clicked();
	on_tbMaxRed_clicked();
	on_tbMinGreen_clicked();
	on_tbMaxGreen_clicked();
	on_tbMinBlue_clicked();
	on_tbMaxBlue_clicked();

}

void WatchVector::newViewCenter(int x, int y)
{
	m_viewCenter[0] = x;
	m_viewCenter[1] = y;
	m_qScrollArea->ensureVisible(m_viewCenter[0], m_viewCenter[1],
			m_qScrollArea->width() / 2 - 1, m_qScrollArea->height() / 2 - 1);
}

void WatchVector::onMinMaxAreaChanged(void)
{
	this->updateGUI();
}

void WatchVector::setWorkspace(QMdiArea *ws)
{
	m_pImageView->setWorkspace(ws);
}

void WatchVector::getActiveChannels(PixelBox *channels[3])
{
	Mapping mappings[3];
	mappings[0] = getMappingFromInt(
			cbRed->itemData(cbRed->currentIndex()).toInt());
	mappings[1] = getMappingFromInt(
			cbGreen->itemData(cbGreen->currentIndex()).toInt());
	mappings[2] = getMappingFromInt(
			cbBlue->itemData(cbBlue->currentIndex()).toInt());
	for (int i = 0; i < 3; i++) {
		if (mappings[i].type != MAP_TYPE_BLACK
				&& mappings[i].type != MAP_TYPE_WHITE) {
			channels[i] = m_pData[mappings[i].index];
		} else {
			channels[i] = NULL;
		}
	}
}

