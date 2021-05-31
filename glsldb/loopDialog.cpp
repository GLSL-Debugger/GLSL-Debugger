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

#include "loopDialog.qt.h"
#include <QtWidgets/QHeaderView>

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QSplitter>
#include <QtCore/QString>
#include <QtGui/QImage>
#include "ShaderLang.h"
#include "colors.qt.h"
#include "vertexTableModel.qt.h"
#include "geoShaderDataModel.qt.h"

#include "dbgprint.h"

/* Fragment Shader */
LoopDialog::LoopDialog(LoopData *data, QWidget *parent) :
		QDialog(parent)
{
	m_pData = data;

	setupUi(this);
	setupGui();

	/* Main GUI window */
	if (m_pData && m_pData->isFragmentLoop()) {
		m_qScrollArea = new QScrollArea();
		m_qScrollArea->setBackgroundRole(QPalette::Dark);
		gridLayout->addWidget(m_qScrollArea);

		m_qLabel = new QLabel();
		m_qLabel->setPixmap(QPixmap::fromImage(m_pData->getImage()));
		m_qScrollArea->setWidget(m_qLabel);
	}
}

/* Geometry Shader */
LoopDialog::LoopDialog(LoopData *data, QList<ShVarItem*> &watchItems,
		int inPrimitiveType, int outPrimitiveType, VertexBox *primitiveMap,
		VertexBox *vertexCountMap, QWidget *parent) :
		QDialog(parent)
{
	int i;
	m_pData = data;

	setupUi(this);
	setupGui();

	/* Main GUI window */
	if (m_pData && m_pData->isVertexLoop()) {
		m_qTreeView = new QTreeView(this);
		m_qTreeView->setAlternatingRowColors(true);

		GeoShaderDataModel *model = new GeoShaderDataModel(inPrimitiveType,
				outPrimitiveType, primitiveMap, vertexCountMap,
				m_pData->getActualVertexBox(), m_pData->getInitialCoverage());

		for (i = 0; i < watchItems.count(); i++) {
			QString name = watchItems[i]->getFullName();
			model->addData(watchItems[i]->getCurrentPointer(),
					watchItems[i]->getVertexBoxPointer(), name);
		}

		m_qTreeView->setModel(model);

		if (watchItems.count() != 0) {
			m_qTreeView->setColumnHidden(1, true);
		}

		gridLayout->addWidget(m_qTreeView);
	}
}

/* Vertex Shader */
LoopDialog::LoopDialog(LoopData *data, QList<ShVarItem*> &watchItems,
		QWidget *parent) :
		QDialog(parent)
{
	int i;
	m_pData = data;

	setupUi(this);
	setupGui();

	/* Main GUI window */
	if (m_pData && m_pData->isVertexLoop()) {
		m_qTableView = new QTableView(this);
		m_qTableView->setAlternatingRowColors(true);

		VertexTableModel *model = new VertexTableModel();
		for (i = 0; i < watchItems.count(); i++) {
			QString name = watchItems[i]->getFullName();
			model->addVertexBox(watchItems[i]->getVertexBoxPointer(), name);
		}
		model->setCondition(m_pData->getActualVertexBox(),
				m_pData->getInitialCoverage());

		m_qTableView->setModel(model);

		if (watchItems.count() != 0) {
			m_qTableView->setColumnHidden(1, true);
		}

		gridLayout->addWidget(m_qTableView);
	}
}

void LoopDialog::setupGui(void)
{
	int i;

	m_qLabel = NULL;
	m_qTreeView = NULL;
	m_qTableView = NULL;

	/* Curves */
	m_qCurves = new CurveView(fCurvesDisplay);
	m_qCurves->setModel(m_pData->getModel());
	m_qCurves->setBase(0);
	m_qCurves->addMapping(1, DBG_GREEN);
	m_qCurves->addMapping(2, DBG_RED);
	m_qCurves->addMapping(3, DBG_ORANGE);

	QVBoxLayout *vboxLayout;
	vboxLayout = new QVBoxLayout(fCurvesDisplay);
	vboxLayout->setSpacing(0);
	vboxLayout->setMargin(0);
	vboxLayout->addWidget(m_qCurves);

	QSplitter *splitter = new QSplitter(Qt::Vertical, tStatistics);
	splitter->addWidget(tvStatTable);
	splitter->addWidget(fCurves);

	QGridLayout *gridStatisticsLayout;
	gridStatisticsLayout = new QGridLayout(tStatistics);
	gridStatisticsLayout->setSpacing(0);
	gridStatisticsLayout->setMargin(0);
	gridStatisticsLayout->addWidget(splitter);

	pbProgress->setMaximum(MAX_LOOP_ITERATIONS);
	pbProgress->setValue(0);
	fProgress->setVisible(false);

	fJump->setVisible(false);

	if (m_pData->getIteration() < MAX_LOOP_ITERATIONS) {
		pbStatistics->setEnabled(true);
		pbJump->setEnabled(true);
		pbNext->setEnabled(true);
	} else {
		pbStatistics->setEnabled(false);
		pbJump->setEnabled(false);
		pbNext->setEnabled(false);
	}

	/* LoopImage/LoopTable */
	QGridLayout *gridLayout;
	gridLayout = new QGridLayout(fContent);
	gridLayout->setSpacing(0);
	gridLayout->setMargin(0);

	/* Statistic text */
	updateIterationStatistics();

	/* Statistics */
	tvStatTable->setModel(m_pData->getModel());
	for (i = 0; i < m_pData->getModel()->columnCount(); i++) {
		tvStatTable->resizeColumnToContents(i);
	}
	for (i = 0; i < m_pData->getModel()->rowCount(); i++) {
		tvStatTable->setRowHeight(i, 22);
	}
	tvStatTable->verticalHeader()->hide();
	connect(m_pData->getModel(),
			SIGNAL(rowsInserted(const QModelIndex&, int, int)), this,
			SLOT(reorganizeLoopTable(const QModelIndex&, int, int)));

	/* Store data for possible skip */
	m_nActive = m_pData->getActive();

}

void LoopDialog::updateIterationStatistics()
{
	/* Iteration Statistics */
	QString string;

	if ((m_pData->getWidth() * m_pData->getHeight())) {
		string = QVariant(m_pData->getTotal()).toString();
		string.append(" (");
		string.append(
				QVariant(
						(m_pData->getTotal() * 100)
								/ ((m_pData->getWidth() * m_pData->getHeight()))).toString());
		string.append("%)");
		lTotalCount->setText(string);
	} else {
		string = QVariant(m_pData->getTotal()).toString();
		lTotalCount->setText(string);
	}

	if (m_pData->getTotal() != 0) {
		string = QVariant(m_pData->getActive()).toString();
		string.append(" (");
		string.append(
				QVariant((m_pData->getActive() * 100) / m_pData->getTotal()).toString());
		string.append("%)");
		lActiveCount->setText(string);
	} else {
		lActiveCount->setText("");
	}
	if (m_pData->getTotal() != 0) {
		string = QVariant(m_pData->getDone()).toString();
		string.append(" (");
		string.append(
				QVariant((m_pData->getDone() * 100) / m_pData->getTotal()).toString());
		string.append("%)");
		lDoneCount->setText(string);
	} else {
		lDoneCount->setText("");
	}
	if (m_pData->getTotal() != 0) {
		string = QVariant(m_pData->getOut()).toString();
		string.append(" (");
		string.append(
				QVariant((m_pData->getOut() * 100) / m_pData->getTotal()).toString());
		string.append("%)");
		lOutCount->setText(string);
	} else {
		lOutCount->setText("");
	}
}

void LoopDialog::on_cbActive_stateChanged(int state)
{
	if (state == Qt::Unchecked) {
		m_qCurves->delMapping(1);
	} else if (state == Qt::Checked) {
		m_qCurves->addMapping(1, DBG_GREEN);
	}
}

void LoopDialog::on_cbDone_stateChanged(int state)
{
	if (state == Qt::Unchecked) {
		m_qCurves->delMapping(2);
	} else if (state == Qt::Checked) {
		m_qCurves->addMapping(2, DBG_RED);
	}
}

void LoopDialog::on_cbOut_stateChanged(int state)
{
	if (state == Qt::Unchecked) {
		m_qCurves->delMapping(3);
	} else if (state == Qt::Checked) {
		m_qCurves->addMapping(3, DBG_ORANGE);
	}
}

void LoopDialog::reorganizeLoopTable(const QModelIndex &parent, int start,
		int end)
{
	int i;
	UNUSED_ARG(parent)
	UNUSED_ARG(start)
	UNUSED_ARG(end)

	QAbstractItemModel *data = tvStatTable->model();
	for (i = 0; i < data->columnCount(); i++) {
		tvStatTable->resizeColumnToContents(i);
	}
	for (i = 0; i < data->rowCount(); i++) {
		tvStatTable->setRowHeight(i, 22);
	}
}

int LoopDialog::exec()
{
	if (m_nActive == 0) {
		return SA_BREAK;
	}

	return QDialog::exec();
}

void LoopDialog::on_pbBreak_clicked()
{
	done(SA_BREAK);
}

void LoopDialog::on_pbStatistics_clicked()
{
	/* Initialization */
	pbBreak->setEnabled(false);
	pbStatistics->setEnabled(false);
	pbJump->setEnabled(false);
	pbNext->setEnabled(false);
	pbProgress->setValue(m_pData->getIteration());
	fProgress->setVisible(true);
	m_bProcessStopped = false;
	tvStatTable->setUpdatesEnabled(false);
	fCurvesDisplay->setUpdatesEnabled(false);

	/* Process loop */
	while ((m_pData->getIteration() < MAX_LOOP_ITERATIONS - 1)
			&& (m_pData->getActive() > 0) && !m_bProcessStopped) {
		emit doShaderStep(DBG_BH_LOOP_NEXT_ITER, false, true);
		pbProgress->setValue(m_pData->getIteration());
		updateIterationStatistics();
		qApp->processEvents(QEventLoop::AllEvents);
	}

	tvStatTable->setUpdatesEnabled(true);
	fCurvesDisplay->setUpdatesEnabled(true);

	emit doShaderStep(DBG_BH_LOOP_NEXT_ITER, true, true);
	if (m_pData->isFragmentLoop()) {
		m_qLabel->setPixmap(QPixmap::fromImage(m_pData->getImage()));
	}
	qApp->processEvents(QEventLoop::AllEvents);

	/* Restore valid GUI state */
	if (!(m_pData->getIteration() < MAX_LOOP_ITERATIONS)) {
		pbBreak->setEnabled(true);
		pbStatistics->setEnabled(false);
		pbJump->setEnabled(false);
		pbNext->setEnabled(false);
		fProgress->setVisible(false);
	} else if (m_pData->getActive() == 0) {
		pbBreak->setEnabled(true);
		pbStatistics->setEnabled(false);
		pbJump->setEnabled(false);
		pbNext->setEnabled(false);
		fProgress->setVisible(false);
	} else {
		pbBreak->setEnabled(true);
		pbStatistics->setEnabled(true);
		pbJump->setEnabled(true);
		pbNext->setEnabled(true);
		fProgress->setVisible(false);
	}
}

void LoopDialog::on_pbJump_clicked()
{
	if (fJump->isVisible()) {
		int i;
		pbBreak->setEnabled(false);
		pbStatistics->setEnabled(false);
		pbJump->setEnabled(false);
		pbNext->setEnabled(false);
		for (i = m_pData->getIteration(); i < hsJump->sliderPosition(); i++) {
			emit doShaderStep(DBG_BH_LOOP_NEXT_ITER, false, false);
		}
		done(SA_JUMP);
	} else {
		pbBreak->setEnabled(false);
		pbStatistics->setEnabled(false);
		pbJump->setEnabled(true);
		pbNext->setEnabled(false);

		hsJump->setMinimum(m_pData->getIteration() + 1);
		hsJump->setMaximum(MAX_LOOP_ITERATIONS);
		hsJump->setSliderPosition(m_pData->getIteration() + 1);

		fJump->setVisible(true);
	}
}

void LoopDialog::on_pbNext_clicked()
{
	done(SA_NEXT);
}

void LoopDialog::on_pbStopProgress_clicked()
{
	m_bProcessStopped = true;
}

void LoopDialog::on_pbStopJump_clicked()
{
	fJump->setVisible(false);

	pbBreak->setEnabled(true);
	if (m_pData->getIteration() < MAX_LOOP_ITERATIONS) {
		pbStatistics->setEnabled(true);
		pbJump->setEnabled(true);
		pbNext->setEnabled(true);
	}
}
