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

#include <QtGui/QDesktopServices>
#include <QtGui/QTextDocument>
#include <QtGui/QMessageBox>
#include <QtGui/QHeaderView>
#include <QtGui/QGridLayout>
#include <QtGui/QFileDialog>
#include <QtCore/QSettings>
#include <QtGui/QWorkspace>
#include <QtGui/QTabWidget>
#include <QtGui/QTabBar>
#include <QtGui/QColor>
#include <QtCore/QUrl>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif /* _WIN32 */

#include "mainWindow.qt.h"
#include "editCallDialog.qt.h"
#include "selectionDialog.qt.h"
#include "dbgShaderView.qt.h"
#include "watchTable.qt.h"
#include "watchVector.qt.h"
#include "watchGeoDataTree.qt.h"
#include "compilerErrorDialog.qt.h"
#include "attachToProcessDialog.qt.h"
#include "aboutBox.qt.h"

#include "glslSyntaxHighlighter.qt.h"
#include "runLevel.h"
#include "debuglib.h"
#include "utils/dbgprint.h"
#include "utils/notify.h"

#define MAX(a,b) ( a < b ? b : a )
#define MIN(a,b) ( a > b ? b : a )

#ifdef _WIN32
#define REGISTRY_KEY "Software\\VIS\\glslDevil"
#endif /* _WIN32 */

extern "C" GLFunctionList glFunctions[];

MainWindow::MainWindow(char *pname, const QStringList& args) :
		dbgProgArgs(args)
{
	int i;

	/*** Setup GUI ****/
	setupUi(this);
	teVertexShader->setTabStopWidth(30);
	teGeometryShader->setTabStopWidth(30);
	teFragmentShader->setTabStopWidth(30);

	/* Functionality seems to be obsolete now */
	tbToggleGuiUpdate->setVisible(false);

	setAttribute(Qt::WA_QuitOnClose, true);

	/*   Status bar    */
	statusbar->addPermanentWidget(fSBError);
	statusbar->addPermanentWidget(fSBMouseInfo);

	/*   Workspace    */
	workspace = new QWorkspace;
	setCentralWidget(workspace);
	connect(workspace, SIGNAL(windowActivated(QWidget*)), this,
			SLOT(changedActiveWindow(QWidget*)));

	/*   Buffer View    */
	QGridLayout *gridLayout;
	gridLayout = new QGridLayout(fContent);
	gridLayout->setSpacing(0);
	gridLayout->setMargin(0);
	sBVArea = new QScrollArea();
	sBVArea->setBackgroundRole(QPalette::Dark);
	gridLayout->addWidget(sBVArea);
	lBVLabel = new QLabel();
	sBVArea->setWidget(lBVLabel);

	/* GLTrace Settings */
	m_pGlTraceFilterModel = new GlTraceFilterModel(glFunctions, this);
	m_pgtDialog = new GlTraceSettingsDialog(m_pGlTraceFilterModel, this);

	/*   GLTrace View   */
	GlTraceListFilterModel *lvGlTraceFilter = new GlTraceListFilterModel(
			m_pGlTraceFilterModel, this);
	m_pGlTraceModel = new GlTraceListModel(MAX_GLTRACE_ENTRIES,
			m_pGlTraceFilterModel, this);
	lvGlTraceFilter->setSourceModel(m_pGlTraceModel);
	lvGlTrace->setModel(lvGlTraceFilter);

	/* Action Group for watch window controls */
	agWatchControl = new QActionGroup(this);
	agWatchControl->addAction(aZoom);
	agWatchControl->addAction(aSelectPixel);
	agWatchControl->addAction(aMinMaxLens);
	agWatchControl->setEnabled(false);

	/* per frgamnet operations */
	m_pftDialog = new FragmentTestDialog(this);

	pc = new ProgramControl(pname);

	m_pCurrentCall = NULL;
	m_pShVarModel = NULL;

	if (dbgProgArgs.size())
		setRunLevel(RL_SETUP);
	else
		setRunLevel(RL_INIT);

	m_bInDLCompilation = false;

	m_pGlCallSt = new GlCallStatistics(tvGlCalls);
	m_pGlExtSt = new GlCallStatistics(tvGlExt);
	m_pGlCallPfst = new GlCallStatistics(tvGlCallsPf);
	m_pGlExtPfst = new GlCallStatistics(tvGlExtPf);
	m_pGlxCallSt = new GlCallStatistics(tvGlxCalls);
	m_pGlxExtSt = new GlCallStatistics(tvGlxExt);
	m_pGlxCallPfst = new GlCallStatistics(tvGlxCallsPf);
	m_pGlxExtPfst = new GlCallStatistics(tvGlxExtPf);
	m_pWglCallSt = new GlCallStatistics(tvWglCalls);
	m_pWglExtSt = new GlCallStatistics(tvWglExt);
	m_pWglCallPfst = new GlCallStatistics(tvWglCallsPf);
	m_pWglExtPfst = new GlCallStatistics(tvWglExtPf);

	/* Prepare debugging */
	ShInitialize();
	m_dShCompiler = 0;

	while (twGlStatistics->count() > 0) {
		twGlStatistics->removeTab(0);
	}
	twGlStatistics->insertTab(0, taGlCalls, QString("GL Calls"));
	twGlStatistics->insertTab(1, taGlExt, QString("GL Extensions"));

	for (i = 0; i < 3; i++) {
		m_pShaders[i] = NULL;
	}
	m_bHaveValidShaderCode = false;

	m_serializedUniforms.pData = NULL;
	m_serializedUniforms.count = 0;

	m_primitiveMode = GL_NONE;

	m_pGeometryMap = NULL;
	m_pVertexCount = NULL;
	//m_pGeoDataModel = NULL;

	m_pCoverage = NULL;

	m_selectedPixel[0] = -1;
	m_selectedPixel[1] = -1;
	lWatchSelectionPos->setText("No Selection");

#ifdef _WIN32
	// TODO: Only Windows can attach at the moment.
	//this->aAttach->setEnabled(true);
#endif /* _WIN32 */

	QSettings settings;
	if (settings.contains("MainWinState")) {
		this->restoreState(settings.value("MainWinState").toByteArray());
	}
}

MainWindow::~MainWindow()
{
	/* Stop still running progs */
	UT_NOTIFY(LV_TRACE, "~MainWindow kill program");
	killProgram(1);

	/* Free reachable memory */
	UT_NOTIFY(LV_TRACE, "~MainWindow free pc");
	delete pc;
	delete m_pGlCallSt;
	delete m_pGlExtSt;
	delete m_pGlCallPfst;
	delete m_pGlExtPfst;
	delete m_pGlxCallSt;
	delete m_pGlxExtSt;
	delete m_pGlxCallPfst;
	delete m_pGlxExtPfst;
	delete m_pWglCallSt;
	delete m_pWglExtSt;
	delete m_pWglCallPfst;
	delete m_pWglExtPfst;

	delete[] m_pCoverage;
	delete m_pGeometryMap;
	delete m_pVertexCount;
	//delete m_pGeoDataModel;

	delete m_pCurrentCall;

	for (int i = 0; i < 3; i++) {
		delete[] m_pShaders[i];
	}

	delete[] m_serializedUniforms.pData;
	m_serializedUniforms.pData = NULL;
	m_serializedUniforms.count = 0;

	/* clean up compiler stuff */
	ShFinalize();
}

void MainWindow::killProgram(int hard)
{
	UT_NOTIFY(LV_TRACE, "Killing debugee");
	pc->killProgram(hard);
	/* status log */
	if (hard) {
		setStatusBarText(QString("Program termination forced!"));
		addGlTraceWarningItem("Program termination forced!");
	} else {
		setStatusBarText(QString("Program terminated!"));
		addGlTraceWarningItem("Program terminated!");
	}
}

void MainWindow::on_aQuit_triggered()
{
	UT_NOTIFY(LV_TRACE, "Quitting application");
	close();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	QSettings settings;
	settings.setValue("MainWinState", this->saveState());
	killProgram(1);
	//qApp->quit();
	event->accept();
}

void MainWindow::on_aOpen_triggered()
{
	Dialog_OpenProgram *dOpenProgram = new Dialog_OpenProgram();
	int i;

	/* Cleanup shader debugging */
	cleanupDBGShader();

	if (dbgProgArgs.size() != 0) {
		QString arguments = QString("");

		dOpenProgram->leProgram->setText(dbgProgArgs[0]);

		for (i = 1; i < dbgProgArgs.size(); i++) {
			arguments.append(dbgProgArgs[i]);
			if (i < dbgProgArgs.size() - 1) {
				arguments.append(" ");
			}
		}
		dOpenProgram->leArguments->setText(arguments);

		dOpenProgram->leWorkDir->setText(this->workDir);

	} else {
		/* Set MRU program, if no old value was available. */
		QString program, arguments, workDir;

		if (this->loadMruProgram(program, arguments, workDir)) {
			dOpenProgram->leProgram->setText(program);
			dOpenProgram->leArguments->setText(arguments);
			dOpenProgram->leWorkDir->setText(workDir);
		}
	}

	dOpenProgram->exec();

	if (dOpenProgram->result() == QDialog::Accepted) {
		if (!(dOpenProgram->leProgram->text().isEmpty())) {
			dbgProgArgs.clear();
			dbgProgArgs.append(dOpenProgram->leProgram->text());
			dbgProgArgs += dOpenProgram->leArguments->text().split(
					QRegExp("\\s+"), QString::SkipEmptyParts);
			/* cleanup dbg state */
			leaveDBGState();
			/* kill program */
			killProgram(1);
			setRunLevel(RL_SETUP);
			setErrorStatus(PCE_NONE);
			setStatusBarText(
					QString("New program: ") + dOpenProgram->leProgram->text());
			clearGlTraceItemList();
		}

		if (!dOpenProgram->leWorkDir->text().isEmpty()) {
			this->workDir = dOpenProgram->leWorkDir->text();
		} else {
			this->workDir.clear();
		}

		/* Save MRU program. */
		this->saveMruProgram(dOpenProgram->leProgram->text(),
				dOpenProgram->leArguments->text(),
				dOpenProgram->leWorkDir->text());
	}
	delete dOpenProgram;
}

void MainWindow::on_aAttach_triggered()
{
	UT_NOTIFY(LV_TRACE, "Quitting application");

	pcErrorCode errorCode;
	Dialog_AttachToProcess dlgAttach(this);
	dlgAttach.exec();

	if (dlgAttach.result() == QDialog::Accepted) {
		ProcessSnapshotModel::Item *process = dlgAttach.getSelectedItem();

		if ((process != NULL) && process->IsAttachtable()) {
			this->dbgProgArgs.clear();
			this->dbgProgArgs.append(QString(process->GetExe()));
			/* cleanup dbg state */
			this->leaveDBGState();
			/* kill program */
			this->killProgram(1);
			this->setRunLevel(RL_SETUP);
			this->setErrorStatus(PCE_NONE);
			this->setStatusBarText(
					QString("New program: ") + QString(process->GetExe()));
			this->clearGlTraceItemList();

			/* Attach to programm. */
			errorCode = this->pc->attachToProgram(process->GetPid());
			if (errorCode != PCE_NONE) {
				QMessageBox::critical(this, "Error", "Attaching to process "
						"failed.");
				this->setRunLevel(RL_INIT);
				this->setErrorStatus(errorCode);
			} else {
				//this->setRunLevel(RL_SETUP);
				this->setErrorStatus(PCE_NONE);
			}
		}
	}
}

void MainWindow::on_aOnlineHelp_triggered()
{
	QUrl url("http://www.vis.uni-stuttgart.de/glsldevil/");
	QDesktopServices ds;
	ds.openUrl(url);
}

void MainWindow::on_aAbout_triggered()
{
	Dialog_AboutBox dlg(this);
	dlg.exec();
}

void MainWindow::setGlStatisticTabs(int n, int m)
{
	while (twGlStatistics->count() > 0) {
		twGlStatistics->removeTab(0);
	}

	switch (n) {
	case 0:
		switch (m) {
		case 0:
			twGlStatistics->insertTab(0, taGlCalls, QString("GL Calls"));
			twGlStatistics->insertTab(1, taGlExt, QString("GL Extensions"));
			break;
		case 1:
			twGlStatistics->insertTab(0, taGlCallsPf, QString("GL Calls"));
			twGlStatistics->insertTab(1, taGlExtPf, QString("GL Extensions"));
			break;
		}
		break;
	case 1:
		switch (m) {
		case 0:
			twGlStatistics->insertTab(0, taGlxCalls, QString("GLX Calls"));
			twGlStatistics->insertTab(1, taGlxExt, QString("GLX Extensions"));
			break;
		case 1:
			twGlStatistics->insertTab(0, taGlxCallsPf, QString("GLX Calls"));
			twGlStatistics->insertTab(1, taGlxExtPf, QString("GLX Extensions"));
			break;
		}
		break;
	case 2:
		switch (m) {
		case 0:
			twGlStatistics->insertTab(0, taWglCalls, QString("WGL Calls"));
			twGlStatistics->insertTab(1, taWglExt, QString("WGL Extensions"));
			break;
		case 1:
			twGlStatistics->insertTab(0, taWglCallsPf, QString("WGL Calls"));
			twGlStatistics->insertTab(1, taWglExtPf, QString("WGL Extensions"));
			break;
		}
		break;
	default:
		break;
	}
}

void MainWindow::on_tbBVCapture_clicked()
{
	int width, height;
	float *imageData;
	pcErrorCode error;

	error = pc->readBackActiveRenderBuffer(3, &width, &height, &imageData);

	if (error == PCE_NONE) {
		PixelBoxFloat imageBox(width, height, 3, imageData);
		lBVLabel->setPixmap(
				QPixmap::fromImage(imageBox.getByteImage(PixelBox::FBM_CLAMP)));
		lBVLabel->resize(width, height);
		tbBVSave->setEnabled(true);
	} else {
		setErrorStatus(error);
	}
}

void MainWindow::on_tbBVCaptureAutomatic_toggled(bool b)
{
	if (b) {
		tbBVCapture->setEnabled(false);
		on_tbBVCapture_clicked();
	} else {
		tbBVCapture->setEnabled(true);
	}
}

void MainWindow::on_tbBVSave_clicked()
{
	static QStringList history;
	static QDir directory = QDir::current();

	if (lBVLabel->pixmap()) {
		QImage img = lBVLabel->pixmap()->toImage();
		if (!img.isNull()) {
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
			sDialog->setFilters(formatDesc);

			if (!(history.isEmpty())) {
				sDialog->setHistory(history);
			}

			sDialog->setDirectory(directory);

			if (sDialog->exec()) {
				QStringList files = sDialog->selectedFiles();
				QString selected;
				if (!files.isEmpty()) {
					selected = files[0];
					if (!(img.save(selected))) {

						QString forceFilter;
						QString filter = sDialog->selectedFilter();
						if (filter
								== QString(
										"Portable Network Graphics (*.png)")) {
							forceFilter.append("png");
						} else if (filter
								== QString("Windows Bitmap (*.bmp)")) {
							forceFilter.append("bmp");
						} else if (filter
								== QString(
										"Joint Photographic Experts Group (*.jpg, *.jepg)")) {
							forceFilter.append("jpg");
						} else if (filter
								== QString("Portable Pixmap (*.ppm)")) {
							forceFilter.append("ppm");
						} else if (filter
								== QString(
										"Tagged Image File Format (*.tif, *.tiff)")) {
							forceFilter.append("tif");
						} else if (filter
								== QString("X11 Bitmap (*.xbm, *.xpm)")) {
							forceFilter.append("xbm");
						}

						img.save(selected, forceFilter.toLatin1().data());
					}
				}
			}

			history = sDialog->history();
			directory = sDialog->directory();

			delete sDialog;
		}
	}
}

void MainWindow::on_cbGlstCallOrigin_currentIndexChanged(int n)
{
	setGlStatisticTabs(n, cbGlstPfMode->currentIndex());
}

void MainWindow::on_cbGlstPfMode_currentIndexChanged(int m)
{
	setGlStatisticTabs(cbGlstCallOrigin->currentIndex(), m);
}

pcErrorCode MainWindow::getNextCall()
{
	m_pCurrentCall = pc->getCurrentCall();
	if (!m_bHaveValidShaderCode && m_pCurrentCall->isDebuggableDrawCall()) {
		/* current call is a drawcall and we don't have valid shader code;
		 * call debug function that reads back the shader code
		 */
		for (int i = 0; i < 3; i++) {
			delete[] m_pShaders[i];
			m_pShaders[i] = NULL;
		}
		delete[] m_serializedUniforms.pData;
		m_serializedUniforms.pData = NULL;
		m_serializedUniforms.count = 0;

		pcErrorCode error = pc->getShaderCode(m_pShaders, &m_dShResources,
				&m_serializedUniforms.pData, &m_serializedUniforms.count);
		if (error == PCE_NONE) {
			/* show shader code(s) in tabs */
			setShaderCodeText(m_pShaders);
			if (m_pShaders[0] != NULL || m_pShaders[1] != NULL
					|| m_pShaders[2] != NULL) {
				m_bHaveValidShaderCode = true;
			}
		} else if (isErrorCritical(error)) {
			return error;
		}
	}
	return PCE_NONE;
}

void MainWindow::on_tbExecute_clicked()
{
	UT_NOTIFY(LV_TRACE, "Executing");
	delete m_pCurrentCall;
	m_pCurrentCall = NULL;

	/* Cleanup shader debugging */
	cleanupDBGShader();

	if (currentRunLevel == RL_SETUP) {
		int i;
		char **args;
		char *workDir;

		/* Clear previous status */
		setStatusBarText(QString(""));
		clearGlTraceItemList();
		resetAllStatistics();

		m_bHaveValidShaderCode = false;

		/* Build arguments */
		args = new char*[dbgProgArgs.size() + 1];
		for (i = 0; i < dbgProgArgs.size(); i++) {
			args[i] = strdup(dbgProgArgs[i].toAscii().data());
		}
		//args[dbgProgArgs.size()] = (char*)malloc(sizeof(char));
		//args[dbgProgArgs.size()] = '\0';
		args[dbgProgArgs.size()] = 0;

		if (this->workDir.isEmpty()) {
			workDir = NULL;
		} else {
			workDir = strdup(this->workDir.toAscii().data());
		}

		/* Execute prog */
		pcErrorCode error = pc->runProgram(args, workDir);

		/* Error handling */
		setErrorStatus(error);
		if (error != PCE_NONE) {
			setRunLevel(RL_SETUP);
		} else {
			setStatusBarText(QString("Executing " + dbgProgArgs[0]));
			m_pCurrentCall = pc->getCurrentCall();
			setRunLevel(RL_TRACE_EXECUTE);
			addGlTraceWarningItem("Program Start");
			addGlTraceItem();
		}

		/* Clean up */
		for (i = 0; i < dbgProgArgs.size() + 1; i++) {
			free(args[i]);
		}
		delete[] args;
		free(workDir);
	} else {
		/* cleanup dbg state */
		leaveDBGState();

		/* Stop already running progs */
		killProgram(1);

		setRunLevel(RL_SETUP);
	}
}

void MainWindow::addGlTraceItem()
{
	if (!m_pCurrentCall)
		return;

	char *callString = m_pCurrentCall->getCallString();
	QIcon icon;
	QString iconText;
	GlTraceListItem::IconType iconType;

	if (currentRunLevel == RL_TRACE_EXECUTE_NO_DEBUGABLE
			|| currentRunLevel == RL_TRACE_EXECUTE_IS_DEBUGABLE) {
		iconType = GlTraceListItem::IT_ACTUAL;
	} else if (currentRunLevel == RL_TRACE_EXECUTE_RUN) {
		iconType = GlTraceListItem::IT_OK;
	} else {
		iconType = GlTraceListItem::IT_EMPTY;
	}

	if (m_pCurrentCall->isGlFunc()) {
		m_pGlCallSt->incCallStatistic(QString(m_pCurrentCall->getName()));
		m_pGlExtSt->incCallStatistic(QString(m_pCurrentCall->getExtension()));
		m_pGlCallPfst->incCallStatistic(QString(m_pCurrentCall->getName()));
		m_pGlExtPfst->incCallStatistic(QString(m_pCurrentCall->getExtension()));
	} else if (m_pCurrentCall->isGlxFunc()) {
		m_pGlxCallSt->incCallStatistic(QString(m_pCurrentCall->getName()));
		m_pGlxExtSt->incCallStatistic(QString(m_pCurrentCall->getExtension()));
		m_pGlxCallPfst->incCallStatistic(QString(m_pCurrentCall->getName()));
		m_pGlxExtPfst->incCallStatistic(
				QString(m_pCurrentCall->getExtension()));
	} else if (m_pCurrentCall->isWglFunc()) {
		m_pWglCallSt->incCallStatistic(QString(m_pCurrentCall->getName()));
		m_pWglExtSt->incCallStatistic(QString(m_pCurrentCall->getExtension()));
		m_pWglCallPfst->incCallStatistic(QString(m_pCurrentCall->getName()));
		m_pWglExtPfst->incCallStatistic(
				QString(m_pCurrentCall->getExtension()));
	}

	/* Check what options are valid depending on the command */
	if (m_pCurrentCall->isDebuggable()) {

	}

	if (m_pGlTraceModel) {
		m_pGlTraceModel->addGlTraceItem(iconType, callString);
	}
	lvGlTrace->scrollToBottom();
	free(callString);
}

void MainWindow::addGlTraceErrorItem(const char *text)
{
	if (m_pGlTraceModel) {
		m_pGlTraceModel->addGlTraceItem(GlTraceListItem::IT_ERROR, text);
	}
	lvGlTrace->scrollToBottom();
}

void MainWindow::addGlTraceWarningItem(const char *text)
{
	if (m_pGlTraceModel) {
		m_pGlTraceModel->addGlTraceItem(GlTraceListItem::IT_WARNING, text);
	}
	lvGlTrace->scrollToBottom();
}

void MainWindow::setGlTraceItemIconType(const GlTraceListItem::IconType type)
{
	if (m_pGlTraceModel) {
		m_pGlTraceModel->setCurrentGlTraceIconType(type, -1);
	}
}

void MainWindow::setGlTraceItemText(const char *text)
{
	if (m_pGlTraceModel) {
		m_pGlTraceModel->setCurrentGlTraceText(text, -1);
	}
}

void MainWindow::clearGlTraceItemList(void)
{
	if (m_pGlTraceModel) {
		m_pGlTraceModel->clear();
	}
}

void MainWindow::setShaderCodeText(char *shaders[3])
{
	if (shaders && shaders[0]) {
		/* make a new document, the old one get's deleted by qt */
		QTextDocument *newDoc = new QTextDocument(QString(shaders[0]),
				teVertexShader);
		/* the document becomes owner of the highlighter, so it get's freed */
		GlslSyntaxHighlighter *highlighter;
		highlighter = new GlslSyntaxHighlighter(newDoc);
		teVertexShader->setDocument(newDoc);
		teVertexShader->setTabStopWidth(30);
	} else {
		/* make a new empty document, the old one get's deleted by qt */
		QTextDocument *newDoc = new QTextDocument(QString(""), teVertexShader);
		teVertexShader->setDocument(newDoc);
	}
	if (shaders && shaders[1]) {
		/* make a new document, the old one get's deleted by qt */
		QTextDocument *newDoc = new QTextDocument(QString(shaders[1]),
				teGeometryShader);
		/* the document becomes owner of the highlighter, so it get's freed */
		GlslSyntaxHighlighter *highlighter;
		highlighter = new GlslSyntaxHighlighter(newDoc);
		teGeometryShader->setDocument(newDoc);
		teGeometryShader->setTabStopWidth(30);
	} else {
		/* make a new empty document, the old one get's deleted by qt */
		QTextDocument *newDoc = new QTextDocument(QString(""),
				teGeometryShader);
		teGeometryShader->setDocument(newDoc);
	}
	if (shaders && shaders[2]) {
		/* make a new document, the old one get's deleted by qt */
		QTextDocument *newDoc = new QTextDocument(QString(shaders[2]),
				teFragmentShader);
		/* the document becomes owner of the highlighter, so it get's freed */
		GlslSyntaxHighlighter *highlighter;
		highlighter = new GlslSyntaxHighlighter(newDoc);
		teFragmentShader->setDocument(newDoc);
		teFragmentShader->setTabStopWidth(30);
	} else {
		/* make a new empty document, the old one get's deleted by qt */
		QTextDocument *newDoc = new QTextDocument(QString(""),
				teFragmentShader);
		teFragmentShader->setDocument(newDoc);
	}
}

pcErrorCode MainWindow::nextStep(const FunctionCall *fCall)
{
	pcErrorCode error;

	if ((fCall && fCall->isShaderSwitch())
			|| m_pCurrentCall->isShaderSwitch()) {
		/* current call is a glsl shader switch */

		/* call shader switch */
		error = pc->callOrigFunc(fCall);

		if (error != PCE_NONE) {
			if (isErrorCritical(error)) {
				return error;
			}
		} else {
			/* call debug function that reads back the shader code */
			for (int i = 0; i < 3; i++) {
				delete[] m_pShaders[i];
				m_pShaders[i] = NULL;
			}
			delete[] m_serializedUniforms.pData;
			m_serializedUniforms.pData = NULL;
			m_serializedUniforms.count = 0;
			error = pc->getShaderCode(m_pShaders, &m_dShResources,
					&m_serializedUniforms.pData, &m_serializedUniforms.count);
			if (error == PCE_NONE) {
				/* show shader code(s) in tabs */
				setShaderCodeText(m_pShaders);
				if (m_pShaders[0] != NULL || m_pShaders[1] != NULL
						|| m_pShaders[2] != NULL) {
					m_bHaveValidShaderCode = true;
				} else {
					m_bHaveValidShaderCode = false;
				}
			} else if (isErrorCritical(error)) {
				return error;
			}
		}
	} else {
		/* current call is a "normal" function call */
		error = pc->callOrigFunc(fCall);
		if (isErrorCritical(error)) {
			return error;
		}
	}
	/* Readback image if requested by user */
	if (tbBVCaptureAutomatic->isChecked()
			&& m_pCurrentCall->isFramebufferChange()) {
		on_tbBVCapture_clicked();
	}
	if (error == PCE_NONE) {
		error = pc->callDone();
	} else {
		/* TODO: what about the error code ??? */
		pc->callDone();
	}
	return error;
}

void MainWindow::on_tbStep_clicked()
{
	leaveDBGState();

	setRunLevel(RL_TRACE_EXECUTE_RUN);

	singleStep();

	if (currentRunLevel != RL_TRACE_EXECUTE_RUN) {
		// a critical error occured in step */
		return;
	}

	while (currentRunLevel == RL_TRACE_EXECUTE_RUN
			&& !m_pGlTraceFilterModel->isFunctionVisible(
					m_pCurrentCall->getName())) {
		singleStep();
		qApp->processEvents(QEventLoop::AllEvents);
		if (currentRunLevel == RL_SETUP) {
			/* something was wrong in step */
			return;
		}
	}

	setRunLevel(RL_TRACE_EXECUTE);
	setGlTraceItemIconType(GlTraceListItem::IT_ACTUAL);
}

void MainWindow::singleStep()
{
	resetPerFrameStatistics();

	/* cleanup dbg state */
	leaveDBGState();

	setGlTraceItemIconType(GlTraceListItem::IT_OK);

	pcErrorCode error = nextStep(NULL);

	delete m_pCurrentCall;
	m_pCurrentCall = NULL;

	/* Error handling */
	setErrorStatus(error);
	if (isErrorCritical(error)) {
		killProgram(1);
		setRunLevel(RL_SETUP);
		return;
	} else {
		if (currentRunLevel == RL_TRACE_EXECUTE_RUN && isOpenGLError(error)
				&& tbToggleHaltOnError->isChecked()) {
			setRunLevel(RL_TRACE_EXECUTE);
		}
		error = getNextCall();
		setErrorStatus(error);
		if (isErrorCritical(error)) {
			killProgram(1);
			setRunLevel(RL_SETUP);
			return;
		}
		if (currentRunLevel != RL_TRACE_EXECUTE_RUN) {
			setRunLevel(RL_TRACE_EXECUTE);
		}
		addGlTraceItem();
	}

}

void MainWindow::on_tbSkip_clicked()
{
	resetPerFrameStatistics();

	/* cleanup dbg state */
	leaveDBGState();

	setGlTraceItemIconType(GlTraceListItem::IT_ERROR);

	pcErrorCode error = pc->callDone();

	delete m_pCurrentCall;
	m_pCurrentCall = NULL;

	/* Error handling */
	setErrorStatus(error);
	if (isErrorCritical(error)) {
		killProgram(1);
		setRunLevel(RL_SETUP);
	} else {
		error = getNextCall();
		setErrorStatus(error);
		if (isErrorCritical(error)) {
			killProgram(1);
			setRunLevel(RL_SETUP);
			return;
		}
		setRunLevel(RL_TRACE_EXECUTE);
		addGlTraceItem();
	}
}

void MainWindow::on_tbEdit_clicked()
{
	EditCallDialog *dialog = new EditCallDialog(m_pCurrentCall);
	dialog->exec();

	if (dialog->result() == QDialog::Accepted) {
		const FunctionCall *editCall = dialog->getChangedFunction();

		resetPerFrameStatistics();

		if ((*(FunctionCall*) editCall) != *m_pCurrentCall) {

			char *callString = editCall->getCallString();
			setGlTraceItemText(callString);
			setGlTraceItemIconType(GlTraceListItem::IT_IMPORTANT);
			free(callString);

			/* Send data to debug library */
			pc->overwriteFuncArguments(editCall);

			/* Replace current call by edited call */
			delete m_pCurrentCall;
			m_pCurrentCall = NULL;

			m_pCurrentCall = new FunctionCall(editCall);
			delete editCall;

		} else {
			/* Just cleanup */
			delete editCall;
		}
	}

	delete dialog;
}

void MainWindow::waitForEndOfExecution()
{
	pcErrorCode error;
	while (currentRunLevel == RL_TRACE_EXECUTE_RUN) {
		qApp->processEvents(QEventLoop::AllEvents);
#ifndef _WIN32
		usleep(1000);
#else /* !_WIN32 */
		Sleep(1);
#endif /* !_WIN32 */
		int state;
		error = pc->checkExecuteState(&state);
		if (isErrorCritical(error)) {
			killProgram(1);
			setRunLevel(RL_SETUP);
			return;
		} else if (isOpenGLError(error)) {
			/* TODO: error check */
			pc->executeContinueOnError();
			pc->checkChildStatus();
			m_pCurrentCall = pc->getCurrentCall();
			addGlTraceItem();
			setErrorStatus(error);
			pc->callDone();
			error = getNextCall();
			setErrorStatus(error);
			if (isErrorCritical(error)) {
				killProgram(1);
				setRunLevel(RL_SETUP);
				return;
			} else {
				setRunLevel(RL_TRACE_EXECUTE);
				addGlTraceItem();
			}
			break;
		}
		if (state) {
			break;
		}
		if (!pc->childAlive()) {
			UT_NOTIFY(LV_INFO, "Debugee terminated!");
			killProgram(0);
			setRunLevel(RL_SETUP);
			return;
		}
	}
	if (currentRunLevel == RL_TRACE_EXECUTE_RUN) {
		pc->checkChildStatus();
		error = getNextCall();
		setRunLevel(RL_TRACE_EXECUTE);
		setErrorStatus(error);
		if (isErrorCritical(error)) {
			killProgram(1);
			setRunLevel(RL_SETUP);
			return;
		} else {
			addGlTraceItem();
		}
	}
}

void MainWindow::on_tbJumpToDrawCall_clicked()
{
	/* cleanup dbg state */
	leaveDBGState();

	setRunLevel(RL_TRACE_EXECUTE_RUN);

	if (tbToggleNoTrace->isChecked()) {
		delete m_pCurrentCall;
		m_pCurrentCall = NULL;

		setShaderCodeText(NULL);
		resetAllStatistics();
		setStatusBarText(QString("Running program without tracing"));
		clearGlTraceItemList();
		addGlTraceWarningItem("Running program without call tracing");
		addGlTraceWarningItem("Statistics reset!");
		qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

		m_bHaveValidShaderCode = false;

		pcErrorCode error = pc->executeToDrawCall(
				tbToggleHaltOnError->isChecked());
		setErrorStatus(error);
		if (error != PCE_NONE) {
			killProgram(1);
			setRunLevel(RL_SETUP);
			return;
		}
		waitForEndOfExecution(); /* last statement!! */
	} else {
		if (m_pCurrentCall && m_pCurrentCall->isDebuggableDrawCall()) {
			singleStep();
			if (currentRunLevel == RL_SETUP) {
				/* something was wrong in step */
				return;
			}
		}

		while (currentRunLevel == RL_TRACE_EXECUTE_RUN
				&& (!m_pCurrentCall
						|| (m_pCurrentCall
								&& !m_pCurrentCall->isDebuggableDrawCall()))) {
			singleStep();
			if (currentRunLevel == RL_SETUP) {
				/* something was wrong in step */
				return;
			}
			qApp->processEvents(QEventLoop::AllEvents);
		}
	}

	setRunLevel(RL_TRACE_EXECUTE);
	setGlTraceItemIconType(GlTraceListItem::IT_ACTUAL);
}

void MainWindow::on_tbJumpToShader_clicked()
{
	/* cleanup dbg state */
	leaveDBGState();

	setRunLevel(RL_TRACE_EXECUTE_RUN);

	if (tbToggleNoTrace->isChecked()) {
		delete m_pCurrentCall;
		m_pCurrentCall = NULL;

		setShaderCodeText(NULL);
		resetAllStatistics();
		setStatusBarText(QString("Running program without tracing"));
		clearGlTraceItemList();
		addGlTraceWarningItem("Running program without call tracing");
		addGlTraceWarningItem("Statistics reset!");
		qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

		m_bHaveValidShaderCode = false;

		pcErrorCode error = pc->executeToShaderSwitch(
				tbToggleHaltOnError->isChecked());
		setErrorStatus(error);
		if (error != PCE_NONE) {
			killProgram(1);
			setRunLevel(RL_SETUP);
			return;
		}
		waitForEndOfExecution(); /* last statement!! */
	} else {
		if (m_pCurrentCall && m_pCurrentCall->isShaderSwitch()) {
			singleStep();
			if (currentRunLevel == RL_SETUP) {
				/* something was wrong in step */
				return;
			}
		}

		while (currentRunLevel == RL_TRACE_EXECUTE_RUN
				&& (!m_pCurrentCall
						|| (m_pCurrentCall && !m_pCurrentCall->isShaderSwitch()))) {
			singleStep();
			if (currentRunLevel == RL_SETUP) {
				/* something was wrong in step */
				return;
			}
			qApp->processEvents(QEventLoop::AllEvents);
		}
	}

	setRunLevel(RL_TRACE_EXECUTE);
	setGlTraceItemIconType(GlTraceListItem::IT_ACTUAL);
}

void MainWindow::on_tbJumpToUserDef_clicked()
{
	static QString targetName;
	JumpToDialog *pJumpToDialog = new JumpToDialog(targetName);

	pJumpToDialog->exec();

	if (pJumpToDialog->result() == QDialog::Accepted) {
		/* cleanup dbg state */
		leaveDBGState();

		setRunLevel(RL_TRACE_EXECUTE_RUN);
		targetName = pJumpToDialog->getTargetFuncName();

		if (tbToggleNoTrace->isChecked()) {
			delete m_pCurrentCall;
			m_pCurrentCall = NULL;

			setShaderCodeText(NULL);
			resetAllStatistics();
			setStatusBarText(QString("Running program without tracing"));
			clearGlTraceItemList();
			addGlTraceWarningItem("Running program without call tracing");
			addGlTraceWarningItem("Statistics reset!");
			qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

			m_bHaveValidShaderCode = false;

			pcErrorCode error = pc->executeToUserDefined(
					targetName.toAscii().data(),
					tbToggleHaltOnError->isChecked());
			setErrorStatus(error);
			if (error != PCE_NONE) {
				killProgram(1);
				setRunLevel(RL_SETUP);
				return;
			}
			waitForEndOfExecution(); /* last statement!! */
		} else {

			singleStep();
			if (currentRunLevel == RL_SETUP) {
				/* something was wrong in step */
				return;
			}

			while (currentRunLevel == RL_TRACE_EXECUTE_RUN
					&& (!m_pCurrentCall
							|| (m_pCurrentCall
									&& targetName.compare(
											m_pCurrentCall->getName())))) {
				singleStep();
				if (currentRunLevel == RL_SETUP) {
					/* something was wrong in step */
					return;
				}
				qApp->processEvents(QEventLoop::AllEvents);
			}
		}

		setRunLevel(RL_TRACE_EXECUTE);
		setGlTraceItemIconType(GlTraceListItem::IT_ACTUAL);
	}

	delete pJumpToDialog;
}

void MainWindow::on_tbRun_clicked()
{
	/* cleanup dbg state */
	leaveDBGState();

	setRunLevel(RL_TRACE_EXECUTE_RUN);
	if (tbToggleNoTrace->isChecked()) {
		delete m_pCurrentCall;
		m_pCurrentCall = NULL;

		setShaderCodeText(NULL);
		resetAllStatistics();
		setStatusBarText(QString("Running program without tracing"));
		clearGlTraceItemList();
		addGlTraceWarningItem("Running program without call tracing");
		addGlTraceWarningItem("Statistics reset!");
		qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

		m_bHaveValidShaderCode = false;

		pcErrorCode error = pc->execute(tbToggleHaltOnError->isChecked());
		setErrorStatus(error);
		if (isErrorCritical(error)) {
			killProgram(1);
			setRunLevel(RL_SETUP);
			return;
		}
		waitForEndOfExecution(); /* last statement !! */
	} else {
		while (currentRunLevel == RL_TRACE_EXECUTE_RUN) {
			singleStep();
			if (currentRunLevel == RL_SETUP) {
				/* something was wrong in step */
				return;
			}
			qApp->processEvents(QEventLoop::AllEvents);
		}
		setGlTraceItemIconType(GlTraceListItem::IT_ACTUAL);
	}
}

void MainWindow::on_tbPause_clicked()
{
	if (tbToggleNoTrace->isChecked()) {
		pc->stop();
	} else {
		setRunLevel(RL_TRACE_EXECUTE);
	}
}

void MainWindow::setGuiUpdates(bool b)
{
	if (b) {
		lvGlTrace->setUpdatesEnabled(true);
		teFragmentShader->setUpdatesEnabled(true);
		teGeometryShader->setUpdatesEnabled(true);
		teVertexShader->setUpdatesEnabled(true);
		tvGlCalls->setUpdatesEnabled(true);
		tvGlCallsPf->setUpdatesEnabled(true);
		tvGlExt->setUpdatesEnabled(true);
		tvGlExtPf->setUpdatesEnabled(true);
		tvGlxCalls->setUpdatesEnabled(true);
		tvGlxCallsPf->setUpdatesEnabled(true);
		tvGlxExt->setUpdatesEnabled(true);
		tvGlxExtPf->setUpdatesEnabled(true);
		tvWglCalls->setUpdatesEnabled(true);
		tvWglCallsPf->setUpdatesEnabled(true);
		tvWglExt->setUpdatesEnabled(true);
		tvWglExtPf->setUpdatesEnabled(true);
	} else {
		lvGlTrace->setUpdatesEnabled(false);
		teFragmentShader->setUpdatesEnabled(false);
		teGeometryShader->setUpdatesEnabled(false);
		teVertexShader->setUpdatesEnabled(false);
		tvGlCalls->setUpdatesEnabled(false);
		tvGlCallsPf->setUpdatesEnabled(false);
		tvGlExt->setUpdatesEnabled(false);
		tvGlExtPf->setUpdatesEnabled(false);
		tvGlxCalls->setUpdatesEnabled(false);
		tvGlxCallsPf->setUpdatesEnabled(false);
		tvGlxExt->setUpdatesEnabled(false);
		tvGlxExtPf->setUpdatesEnabled(false);
		tvWglCalls->setUpdatesEnabled(false);
		tvWglCallsPf->setUpdatesEnabled(false);
		tvWglExt->setUpdatesEnabled(false);
		tvWglExtPf->setUpdatesEnabled(false);
	}
}

void MainWindow::on_tbToggleGuiUpdate_clicked(bool b)
{
	setGuiUpdates(!b);
}

void MainWindow::on_tbToggleNoTrace_clicked(bool)
{
}

void MainWindow::on_tbToggleHaltOnError_clicked(bool)
{
}

void MainWindow::on_tbGlTraceSettings_clicked()
{
	m_pgtDialog->exec();
	m_pGlTraceModel->resetLayout();
}

void MainWindow::on_tbSave_clicked()
{

	static QStringList history;
	static QDir directory = QDir::current();

	QFileDialog *sDialog = new QFileDialog(this, QString("Save GL Trace as"));

	sDialog->setAcceptMode(QFileDialog::AcceptSave);
	sDialog->setFileMode(QFileDialog::AnyFile);
	QStringList formatDesc;
	formatDesc << "Plain Text (*.txt)";
	sDialog->setFilters(formatDesc);

	if (!(history.isEmpty())) {
		sDialog->setHistory(history);
	}

	sDialog->setDirectory(directory);

	if (sDialog->exec()) {
		QStringList files = sDialog->selectedFiles();
		QString selected;
		if (!files.isEmpty()) {
			QString filter = sDialog->selectedFilter();
			if (filter == QString("Plain Text (*.txt)")) {
				QFile file(files[0]);
				if (file.open(QIODevice::WriteOnly)) {

					QTextStream out(&file);
					m_pGlTraceModel->traverse(out, &GlTraceListItem::outputTXT);
				}
			}
		}
	}

	history = sDialog->history();
	directory = sDialog->directory();

	delete sDialog;
}

bool MainWindow::getDebugVertexData(DbgCgOptions option, ShChangeableList *cl,
		bool *coverage, VertexBox *vdata)
{
	int target, elementsPerVertex, numVertices, numPrimitives,
			forcePointPrimitiveMode;
	float *data = NULL;
	pcErrorCode error;

	char *shaders[] = {
		m_pShaders[0],
		m_pShaders[1],
		m_pShaders[2] };

	char *debugCode = NULL;
	debugCode = ShDebugGetProg(m_dShCompiler, cl, &m_dShVariableList, option);
	switch (currentRunLevel) {
	case RL_DBG_VERTEX_SHADER:
		shaders[0] = debugCode;
		shaders[1] = NULL;
		target = DBG_TARGET_VERTEX_SHADER;
		break;
	case RL_DBG_GEOMETRY_SHADER:
		shaders[1] = debugCode;
		target = DBG_TARGET_GEOMETRY_SHADER;
		break;
	default:
		QMessageBox::critical(this, "Internal Error",
				"MainWindow::getDebugVertexData called when debugging "
						"non-vertex/geometry shader<br>Please report this probem to "
						"<A HREF=\"mailto:glsldevil@vis.uni-stuttgart.de\">"
						"glsldevil@vis.uni-stuttgart.de</A>.", QMessageBox::Ok);
		free(debugCode);
		return false;
	}

	switch (option) {
	case DBG_CG_GEOMETRY_MAP:
		elementsPerVertex = 3;
		forcePointPrimitiveMode = 0;
		break;
	case DBG_CG_VERTEX_COUNT:
		elementsPerVertex = 3;
		forcePointPrimitiveMode = 1;
		break;
	case DBG_CG_GEOMETRY_CHANGEABLE:
		elementsPerVertex = 2;
		forcePointPrimitiveMode = 0;
		break;
	case DBG_CG_CHANGEABLE:
	case DBG_CG_COVERAGE:
	case DBG_CG_SELECTION_CONDITIONAL:
	case DBG_CG_SWITCH_CONDITIONAL:
	case DBG_CG_LOOP_CONDITIONAL:
		if (target == DBG_TARGET_GEOMETRY_SHADER) {
			elementsPerVertex = 1;
			forcePointPrimitiveMode = 1;
		} else {
			elementsPerVertex = 1;
			forcePointPrimitiveMode = 0;
		}
		break;
	default:
		elementsPerVertex = 1;
		forcePointPrimitiveMode = 0;
		break;
	}

	error = pc->shaderStepVertex(shaders, target, m_primitiveMode,
			forcePointPrimitiveMode, elementsPerVertex, &numPrimitives,
			&numVertices, &data);

	/////// DEBUG
	UT_NOTIFY(LV_DEBUG, ">>>>> DEBUG CG: ");
	switch (option) {
	case DBG_CG_GEOMETRY_MAP:
		dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "DBG_CG_GEOMETRY_MAP\n");
		break;
	case DBG_CG_VERTEX_COUNT:
		dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "DBG_CG_VERTEX_COUNT\n");
		break;
	case DBG_CG_GEOMETRY_CHANGEABLE:
		dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "DBG_CG_GEOMETRY_CHANGEABLE\n");
		break;
	case DBG_CG_CHANGEABLE:
		dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "DBG_CG_CHANGEABLE\n");
		break;
	case DBG_CG_COVERAGE:
		dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "DBG_CG_COVERAGE\n");
		break;
	case DBG_CG_SELECTION_CONDITIONAL:
		dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "DBG_CG_SELECTION_CONDITIONAL\n");
		break;
	case DBG_CG_SWITCH_CONDITIONAL:
		dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "DBG_CG_SWITCH_CONDITIONAL\n");
		break;
	case DBG_CG_LOOP_CONDITIONAL:
		dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "DBG_CG_LOOP_CONDITIONAL\n");
		break;
	default:
		dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "XXXXXX FIXME XXXXX\n");
		break;
	}
	if (currentRunLevel == RL_DBG_VERTEX_SHADER) {
		dbgPrint(DBGLVL_COMPILERINFO,
				">>>>> DEBUG VERTEX SHADER:\n %s\n", shaders[0]);
	} else {
		dbgPrint(DBGLVL_COMPILERINFO,
				">>>>> DEBUG GEOMETRY SHADER:\n %s\n", shaders[1]);
	}
	dbgPrint(DBGLVL_INFO,
			"getDebugVertexData: numPrimitives=%i numVertices=%i\n", numPrimitives, numVertices);
	////////////////

	free(debugCode);
	if (error != PCE_NONE) {
		setErrorStatus(error);
		if (isErrorCritical(error)) {
			cleanupDBGShader();
			setRunLevel(RL_SETUP);
			QMessageBox::critical(this, "Critical Error", "Could not debug "
					"shader. An error occured!", QMessageBox::Ok);
			UT_NOTIFY(LV_ERROR,
					"Critical Error in getDebugVertexData: " << getErrorDescription(error));
			killProgram(1);
			return false;
		}
		QMessageBox::critical(this, "Error", "Could not debug "
				"shader. An error occured!", QMessageBox::Ok);
		UT_NOTIFY(LV_WARN,
				"Error in getDebugVertexData: " << getErrorDescription(error));
		return false;
	}

	vdata->setData(data, elementsPerVertex, numVertices, numPrimitives,
			coverage);
	free(data);
	UT_NOTIFY(LV_TRACE, "getDebugVertexData done");
	return true;
}

bool MainWindow::getDebugImage(DbgCgOptions option, ShChangeableList *cl,
		int rbFormat, bool *coverage, PixelBox **fbData)
{
	int width, height, channels;
	void *imageData;
	pcErrorCode error;

	char *shaders[] = {
		m_pShaders[0],
		m_pShaders[1],
		m_pShaders[2] };

	if (currentRunLevel != RL_DBG_FRAGMENT_SHADER) {
		QMessageBox::critical(this, "Internal Error",
				"MainWindow::getDebugImage called when debugging "
						"debugging non-fragment shader<br>Please report this probem to "
						"<A HREF=\"mailto:glsldevil@vis.uni-stuttgart.de\">"
						"glsldevil@vis.uni-stuttgart.de</A>.", QMessageBox::Ok);
		return false;
	}

	char *debugCode = NULL;
	debugCode = ShDebugGetProg(m_dShCompiler, cl, &m_dShVariableList, option);
	shaders[2] = debugCode;

	switch (option) {
	case DBG_CG_CHANGEABLE:
	case DBG_CG_COVERAGE:
		channels = 1;
		break;
	case DBG_CG_SELECTION_CONDITIONAL:
	case DBG_CG_SWITCH_CONDITIONAL:
	case DBG_CG_LOOP_CONDITIONAL:
		if (currentRunLevel == RL_DBG_FRAGMENT_SHADER)
			channels = 1;
		else
			channels = 3;
		break;
	default:
		channels = 3;
		break;
	}

	UT_NOTIFY(LV_TRACE, "Init buffers...");
	switch (option) {
	case DBG_CG_ORIGINAL_SRC:
		error = pc->initializeRenderBuffer(true, true, true, true, 0.0, 0.0,
				0.0, 0.0, 0.0, 0);
		break;
	case DBG_CG_COVERAGE:
	case DBG_CG_SELECTION_CONDITIONAL:
	case DBG_CG_SWITCH_CONDITIONAL:
	case DBG_CG_LOOP_CONDITIONAL:
	case DBG_CG_CHANGEABLE:
		error = pc->initializeRenderBuffer(false, m_pftDialog->copyAlpha(),
				m_pftDialog->copyDepth(), m_pftDialog->copyStencil(), 0.0, 0.0,
				0.0, m_pftDialog->alphaValue(), m_pftDialog->depthValue(),
				m_pftDialog->stencilValue());
		break;
	default: {
		QString msg;
		msg.append("Unhandled DbgCgCoption ");
		msg.append(QString::number(option));
		msg.append("<BR>Please report this probem to "
				"<A HREF=\"mailto:glsldevil@vis.uni-stuttgart.de\">"
				"glsldevil@vis.uni-stuttgart.de</A>.");
		QMessageBox::critical(this, "Internal Error", msg, QMessageBox::Ok);
		return false;
	}
	}
	setErrorStatus(error);
	if (isErrorCritical(error)) {
		cleanupDBGShader();
		setRunLevel(RL_SETUP);
		QMessageBox::critical(this, "Error", "Could not initialize buffers for "
				"fragment program debugging.", QMessageBox::Ok);
		killProgram(1);
		return false;
	}

	error = pc->shaderStepFragment(shaders, channels, rbFormat, &width, &height,
			&imageData);
	free(debugCode);
	if (error != PCE_NONE) {
		setErrorStatus(error);
		if (isErrorCritical(error)) {
			cleanupDBGShader();
			setRunLevel(RL_SETUP);
			QMessageBox::critical(this, "Error", "Could not debug fragment "
					"shader. An error occured!", QMessageBox::Ok);
			killProgram(1);
			return false;
		}
		QMessageBox::critical(this, "Error", "Could not debug fragment "
				"shader. An error occured!", QMessageBox::Ok);
		return false;
	}

	if (rbFormat == GL_FLOAT) {
		PixelBoxFloat *fb = new PixelBoxFloat(width, height, channels,
				(float*) imageData, coverage);
		if (*fbData) {
			PixelBoxFloat *pfbData = dynamic_cast<PixelBoxFloat*>(*fbData);
			pfbData->addPixelBox(fb);
			delete fb;
		} else {
			*fbData = fb;
		}
	} else if (rbFormat == GL_INT) {
		PixelBoxInt *fb = new PixelBoxInt(width, height, channels,
				(int*) imageData, coverage);
		if (*fbData) {
			PixelBoxInt *pfbData = dynamic_cast<PixelBoxInt*>(*fbData);
			pfbData->addPixelBox(fb);
			delete fb;
		} else {
			*fbData = fb;
		}
	} else if (rbFormat == GL_UNSIGNED_INT) {
		PixelBoxUInt *fb = new PixelBoxUInt(width, height, channels,
				(unsigned int*) imageData, coverage);
		if (*fbData) {
			PixelBoxUInt *pfbData = dynamic_cast<PixelBoxUInt*>(*fbData);
			pfbData->addPixelBox(fb);
			delete fb;
		} else {
			*fbData = fb;
		}
	} else {
		UT_NOTIFY(LV_ERROR, "Invalid image data format");
	}

	free(imageData);
	UT_NOTIFY(LV_TRACE, "getDebugImage done.");
	return true;
}

void MainWindow::updateWatchItemData(ShVarItem *watchItem)
{
	ShChangeableList cl;

	cl.numChangeables = 0;
	cl.changeables = NULL;

	ShChangeable *watchItemCgbl = watchItem->getShChangeable();
	addShChangeable(&cl, watchItemCgbl);

	int rbFormat = watchItem->getReadbackFormat();

	if (currentRunLevel == RL_DBG_FRAGMENT_SHADER) {
		PixelBox *fb = watchItem->getPixelBoxPointer();
		if (fb) {
			if (getDebugImage(DBG_CG_CHANGEABLE, &cl, rbFormat, m_pCoverage,
					&fb)) {
				watchItem->setCurrentValue(m_selectedPixel[0],
						m_selectedPixel[1]);
			} else {
				QMessageBox::warning(this, "Warning",
						"The requested data could "
								"not be retrieved.");
			}
		} else {
			if (getDebugImage(DBG_CG_CHANGEABLE, &cl, rbFormat, m_pCoverage,
					&fb)) {
				watchItem->setPixelBoxPointer(fb);
				watchItem->setCurrentValue(m_selectedPixel[0],
						m_selectedPixel[1]);
			} else {
				QMessageBox::warning(this, "Warning",
						"The requested data could "
								"not be retrieved.");
			}
		}
	} else if (currentRunLevel == RL_DBG_VERTEX_SHADER) {
		VertexBox *data = new VertexBox();
		if (getDebugVertexData(DBG_CG_CHANGEABLE, &cl, m_pCoverage, data)) {
			VertexBox *vb = watchItem->getVertexBoxPointer();
			if (vb) {
				vb->addVertexBox(data);
				delete data;
			} else {
				watchItem->setVertexBoxPointer(data);
			}
			watchItem->setCurrentValue(m_selectedPixel[0]);
		} else {
			QMessageBox::warning(this, "Warning", "The requested data could "
					"not be retrieved.");
		}
	} else if (currentRunLevel == RL_DBG_GEOMETRY_SHADER) {
		VertexBox *currentData = new VertexBox();

		UT_NOTIFY(LV_TRACE, "Get CHANGEABLE:");
		if (getDebugVertexData(DBG_CG_CHANGEABLE, &cl, m_pCoverage,
				currentData)) {
			VertexBox *vb = watchItem->getCurrentPointer();
			if (vb) {
				vb->addVertexBox(currentData);
				delete currentData;
			} else {
				watchItem->setCurrentPointer(currentData);
			}

			VertexBox *vertexData = new VertexBox();

			UT_NOTIFY(LV_TRACE, "Get GEOMETRY_CHANGABLE:");
			if (getDebugVertexData(DBG_CG_GEOMETRY_CHANGEABLE, &cl, NULL,
					vertexData)) {
				VertexBox *vb = watchItem->getVertexBoxPointer();
				if (vb) {
					vb->addVertexBox(vertexData);
					delete vertexData;
				} else {
					watchItem->setVertexBoxPointer(vertexData);
				}
			} else {
				QMessageBox::warning(this, "Warning",
						"The requested data could "
								"not be retrieved.");
			}
			watchItem->setCurrentValue(m_selectedPixel[0]);
		} else {
			QMessageBox::warning(this, "Warning", "The requested data could "
					"not be retrieved.");
			return;
		}
	}
	freeShChangeable(&watchItemCgbl);
}

static void invalidateWatchItemData(ShVarItem *item)
{
	if (item->getPixelBoxPointer()) {
		item->getPixelBoxPointer()->invalidateData();
		item->resetCurrentValue();
	}
	if (item->getCurrentPointer()) {
		item->getCurrentPointer()->invalidateData();
	}
	if (item->getVertexBoxPointer()) {
		item->getVertexBoxPointer()->invalidateData();
	}
}

void MainWindow::updateWatchListData(CoverageMapStatus cmstatus,
		bool forceUpdate)
{
	QList<ShVarItem*> watchItems;
	int i;

	if (m_pShVarModel) {
		watchItems = m_pShVarModel->getAllWatchItemPointers();
	}

	for (i = 0; i < watchItems.count(); i++) {
		ShVarItem *item = watchItems[i];

		UT_NOTIFY_VA(LV_TRACE,
				">>>>>>>>>>>>>>updateWatchListData: %s (%i, %i, %i, %i)\n", qPrintable(item->getFullName()), item->isChanged(), item->hasEnteredScope(), item->isInScope(), item->isInScopeStack());

		if (forceUpdate) {
			if (item->isInScope() || item->isBuildIn()
					|| item->isInScopeStack()) {
				updateWatchItemData(item);
			} else {
				invalidateWatchItemData(item);
			}
		} else if ((item->isChanged() || item->hasEnteredScope())
				&& (item->isInScope() || item->isInScopeStack())) {
			updateWatchItemData(item);
		} else if (item->hasLeftScope()) {
			invalidateWatchItemData(item);
		} else {
			/* If covermap grows larger, more readbacks could become possible */
			if (cmstatus == COVERAGEMAP_GROWN) {
				if (item->isInScope() || item->isBuildIn()
						|| item->isInScopeStack()) {
					if (currentRunLevel == RL_DBG_FRAGMENT_SHADER) {
						PixelBox *dataBox = item->getPixelBoxPointer();
						if (!(dataBox->isAllDataAvailable())) {
							updateWatchItemData(item);
						}
					} else {
						updateWatchItemData(item);
					}
				} else {
					invalidateWatchItemData(item);
				}
			}
		}
		/* HACK: when an error occurs in shader debugging the runlevel
		 * might change to RL_SETUP and all shader debugging data will
		 * be invalid; so we have to check it here
		 */
		if (currentRunLevel == RL_SETUP) {
			return;
		}
	}

	/* Now update all windows to update themselves if necessary */
	QWidgetList windowList = workspace->windowList();

	for (i = 0; i < windowList.count(); i++) {
		WatchView *wv = static_cast<WatchView*>(windowList[i]);
		wv->updateView(cmstatus != COVERAGEMAP_UNCHANGED);
	}
	/* update view */
	m_pShVarModel->currentValuesChanged();
}

void MainWindow::updateWatchItemsCoverage(bool *coverage)
{
	QList<ShVarItem*> watchItems;
	int i;

	if (m_pShVarModel) {
		watchItems = m_pShVarModel->getAllWatchItemPointers();
	}

	for (i = 0; i < watchItems.count(); i++) {
		ShVarItem *item = watchItems[i];
		if (currentRunLevel == RL_DBG_FRAGMENT_SHADER) {
			PixelBox *fb = item->getPixelBoxPointer();
			fb->setNewCoverage(coverage);
			item->setCurrentValue(m_selectedPixel[0], m_selectedPixel[1]);
		} else if (currentRunLevel == RL_DBG_VERTEX_SHADER) {
			VertexBox *vb = item->getVertexBoxPointer();
			vb->setNewCoverage(coverage);
			item->setCurrentValue(m_selectedPixel[0]);
		} else if (currentRunLevel == RL_DBG_GEOMETRY_SHADER) {
			VertexBox *cb = item->getCurrentPointer();
			cb->setNewCoverage(coverage);
			item->setCurrentValue(m_selectedPixel[0]);
		}
	}
	/* update view */
	m_pShVarModel->currentValuesChanged();
}

void MainWindow::resetWatchListData(void)
{
	QList<ShVarItem*> watchItems;
	int i;

	if (m_pShVarModel) {
		watchItems = m_pShVarModel->getAllWatchItemPointers();
		for (i = 0; i < watchItems.count(); i++) {
			ShVarItem *item = watchItems[i];
			if (item->isInScope() || item->isBuildIn()) {
				updateWatchItemData(item);
				/* HACK: when an error occurs in shader debugging the runlevel
				 * might change to RL_SETUP and all shader debugging data will
				 * be invalid; so we have to check it here
				 */
				if (currentRunLevel == RL_SETUP) {
					return;
				}
			} else {
				invalidateWatchItemData(item);
			}
		}
		/* Now notify all windows to update themselves if necessary */
		QWidgetList windowList = workspace->windowList();
		for (i = 0; i < windowList.count(); i++) {
			WatchView *wv = static_cast<WatchView*>(windowList[i]);
			wv->updateView(true);
		}
		/* update view */
		m_pShVarModel->currentValuesChanged();
	}
}

void MainWindow::ShaderStep(int action, bool updateWatchData,
		bool updateCovermap)
{
	bool updateGUI = true;

	switch (action) {
	case DBG_BH_RESET:
	case DBG_BH_JUMP_INTO:
	case DBG_BH_FOLLOW_ELSE:
	case DBG_BH_JUMP_OVER:
		updateGUI = true;
		break;
	case DBG_BH_LOOP_NEXT_ITER:
		updateGUI = false;
		break;
	}

	int debugOptions = EDebugOpIntermediate;
	DbgResult *dr = NULL;
	static int nOldCoverageMap = 0;
	CoverageMapStatus cmstatus = COVERAGEMAP_UNCHANGED;

	dr = ShDebugJumpToNext(m_dShCompiler, debugOptions, action);

	if (dr) {
		switch (dr->status) {
		case DBG_RS_STATUS_OK: {
			/* Update scope list and mark changed variables */
			m_pShVarModel->setChangedAndScope(dr->cgbls, dr->scope,
					dr->scopeStack);

			if (currentRunLevel == RL_DBG_FRAGMENT_SHADER && updateCovermap) {
				/* Read cover map */
				PixelBoxFloat *pCoverageBox = NULL;
				if (!(getDebugImage(DBG_CG_COVERAGE, NULL, GL_FLOAT, NULL,
						(PixelBox**) &pCoverageBox))) {
					QMessageBox::warning(this, "Warning", "An error "
							"occurred while reading coverage.");
					return;
				}

				/* Retrieve covermap from CoverageBox */
				int nNewCoverageMap;
				delete[] m_pCoverage;
				m_pCoverage = pCoverageBox->getCoverageFromData(
						&nNewCoverageMap);
				updateWatchItemsCoverage(m_pCoverage);

				if (nNewCoverageMap == nOldCoverageMap) {
					cmstatus = COVERAGEMAP_UNCHANGED;
				} else if (nNewCoverageMap > nOldCoverageMap) {
					cmstatus = COVERAGEMAP_GROWN;
				} else {
					cmstatus = COVERAGEMAP_SHRINKED;
				}
				nOldCoverageMap = nNewCoverageMap;

				delete pCoverageBox;
			} else if ((currentRunLevel == RL_DBG_GEOMETRY_SHADER
					|| (currentRunLevel == RL_DBG_VERTEX_SHADER))
					&& updateCovermap) {
				/* Retrieve cover map (one render pass 'DBG_CG_COVERAGE') */
				VertexBox *pCoverageBox = new VertexBox(NULL);
				if (!(getDebugVertexData(DBG_CG_COVERAGE, NULL, NULL,
						pCoverageBox))) {
					QMessageBox::warning(this, "Warning", "An error "
							"occurred while reading vertex coverage.");
					/* TODO: error handling */
					UT_NOTIFY(LV_WARN, "Error reading vertex coverage!");
					delete pCoverageBox;
					cleanupDBGShader();
					setRunLevel(RL_DBG_RESTART);
					return;
				}

				/* Convert data to bool map: VertexBox -> bool
				 * Check for change of covermap */
				bool *newCoverage;
				bool coverageChanged;
				newCoverage = pCoverageBox->getCoverageFromData(m_pCoverage,
						&coverageChanged);
				delete[] m_pCoverage;
				m_pCoverage = newCoverage;
				updateWatchItemsCoverage(m_pCoverage);

				if (coverageChanged) {
					UT_NOTIFY(LV_INFO, "cmstatus = COVERAGEMAP_GROWN");
					cmstatus = COVERAGEMAP_GROWN;
				} else {
					UT_NOTIFY(LV_INFO, "cmstatus = COVERAGEMAP_UNCHANGED");
					cmstatus = COVERAGEMAP_UNCHANGED;
				}

				delete pCoverageBox;
			}
		}
			break;
		case DBG_RS_STATUS_FINISHED:
			tbShaderStep->setEnabled(false);
			tbShaderStepOver->setEnabled(false);
			break;
		default: {
			QString msg;
			msg.append("An unhandled debug result (");
			msg.append(QString::number(dr->status));
			msg.append(") occurred.");
			msg.append("<br>Please report this probem to "
					"<A HREF=\"mailto:glsldevil@vis.uni-stuttgart.de\">"
					"glsldevil@vis.uni-stuttgart.de</A>.");
			QMessageBox::critical(this, "Internal Error", msg, QMessageBox::Ok);
			return;
		}
			break;
		}

		if (dr->position == DBG_RS_POSITION_DUMMY) {
			tbShaderStep->setEnabled(false);
			tbShaderStepOver->setEnabled(false);
		}

		/* Process watch list */
		if (updateWatchData) {
			UT_NOTIFY(LV_INFO,
					"updateWatchData " << cmstatus << " emitVertex: " << dr->passedEmitVertex << " discard: " << dr->passedDiscard);
			updateWatchListData(cmstatus,
					dr->passedEmitVertex || dr->passedDiscard);
		}

		VertexBox vbCondition;

		/* Process position dependent requests */
		switch (dr->position) {
		case DBG_RS_POSITION_SELECTION_IF_CHOOSE:
		case DBG_RS_POSITION_SELECTION_IF_ELSE_CHOOSE: {
			SelectionDialog *sDialog = NULL;
			switch (currentRunLevel) {
			case RL_DBG_FRAGMENT_SHADER: {
				PixelBoxFloat *imageBox = NULL;
				if (getDebugImage(DBG_CG_SELECTION_CONDITIONAL, NULL, GL_FLOAT,
						m_pCoverage, (PixelBox**) &imageBox)) {
				} else {
					QMessageBox::warning(this, "Warning",
							"An error occurred while retrieving "
									"the selection image.");
					delete imageBox;
					return;
				}
				sDialog = new SelectionDialog(imageBox,
						dr->position
								== DBG_RS_POSITION_SELECTION_IF_ELSE_CHOOSE,
						this);
				delete imageBox;
			}
				break;
			case RL_DBG_GEOMETRY_SHADER: {
				if (getDebugVertexData(DBG_CG_SELECTION_CONDITIONAL, NULL,
						m_pCoverage, &vbCondition)) {
				} else {
					QMessageBox::warning(this, "Warning",
							"An error occurred while retrieving "
									"the selection condition.");
					cleanupDBGShader();
					setRunLevel(RL_DBG_RESTART);
					return;
				}

				/* Create list of all watch item boxes */
				QList<ShVarItem*> watchItems;
				if (m_pShVarModel) {
					watchItems = m_pShVarModel->getAllWatchItemPointers();
				}

				sDialog = new SelectionDialog(&vbCondition, watchItems,
						m_primitiveMode, m_dShResources.geoOutputType,
						m_pGeometryMap, m_pVertexCount,
						dr->position
								== DBG_RS_POSITION_SELECTION_IF_ELSE_CHOOSE,
						this);
			}
				break;
			case RL_DBG_VERTEX_SHADER: {
				/* Get condition for each vertex */
				if (getDebugVertexData(DBG_CG_SELECTION_CONDITIONAL, NULL,
						m_pCoverage, &vbCondition)) {
				} else {
					QMessageBox::warning(this, "Warning",
							"An error occurred while retrieving "
									"the selection condition.");
					cleanupDBGShader();
					setRunLevel(RL_DBG_RESTART);
					return;
				}

				/* Create list of all watch item boxes */
				QList<ShVarItem*> watchItems;
				if (m_pShVarModel) {
					watchItems = m_pShVarModel->getAllWatchItemPointers();
				}

				sDialog = new SelectionDialog(&vbCondition, watchItems,
						dr->position
								== DBG_RS_POSITION_SELECTION_IF_ELSE_CHOOSE,
						this);
			}
				break;
			default:
				// TODO: Is this an internal error?
				QMessageBox::warning(this, "Warning",
						"The current run level is invalid for "
								"SelectionDialog.");
			}
			switch (sDialog->exec()) {
			case SelectionDialog::SB_SKIP:
				ShaderStep (DBG_BH_JUMP_OVER);
				break;
			case SelectionDialog::SB_IF:
				ShaderStep (DBG_BH_JUMP_INTO);
				break;
			case SelectionDialog::SB_ELSE:
				ShaderStep (DBG_BH_FOLLOW_ELSE);
				break;
			}
			delete sDialog;
		}
			break;
		case DBG_RS_POSITION_SWITCH_CHOOSE: {
			// TODO: switch choose
		}
			break;
		case DBG_RS_POSITION_LOOP_CHOOSE: {
			LoopData *lData = NULL;
			switch (currentRunLevel) {
			case RL_DBG_FRAGMENT_SHADER: {
				PixelBoxFloat *loopCondition = NULL;

				if (updateCovermap) {
					/* First get image of loop condition */
					if (!getDebugImage(DBG_CG_LOOP_CONDITIONAL, NULL, GL_FLOAT,
							m_pCoverage, (PixelBox**) &loopCondition)) {
						QMessageBox::warning(this, "Warning",
								"An error occurred while retrieving "
										"the loop image.");
						delete loopCondition;
						return;
					}
				}

				/* Add data to the loop storage */
				if (dr->loopIteration == 0) {
					UT_NOTIFY(LV_INFO, "==> new loop encountered");
					lData = new LoopData(loopCondition, this);
					m_qLoopData.push(lData);
				} else {
					UT_NOTIFY(LV_INFO,
							"==> known loop at " << dr->loopIteration);
					if (!m_qLoopData.isEmpty()) {
						lData = m_qLoopData.top();
						if (updateCovermap) {
							lData->addLoopIteration(loopCondition,
									dr->loopIteration);
						}
					} else {
						/* TODO error handling */
						QMessageBox::warning(this, "Warning",
								"An error occurred while trying to "
										"get loop count data.");
						ShaderStep (DBG_BH_JUMP_OVER);
						delete loopCondition;
						return;
					}
				}
				delete loopCondition;
			}
				break;
			case RL_DBG_VERTEX_SHADER:
			case RL_DBG_GEOMETRY_SHADER: {
				VertexBox loopCondition;
				if (!(getDebugVertexData(DBG_CG_LOOP_CONDITIONAL, NULL,
						m_pCoverage, &loopCondition))) {
					QMessageBox::warning(this, "Warning",
							"An error occurred while trying to "
									"get the loop count condition.");
					cleanupDBGShader();
					setRunLevel(RL_DBG_RESTART);
					return;
				}

				/* Add data to the loop storage */
				if (dr->loopIteration == 0) {
					UT_NOTIFY(LV_INFO, "==> new loop encountered\n");
					lData = new LoopData(&loopCondition, this);
					m_qLoopData.push(lData);
				} else {
					UT_NOTIFY(LV_INFO,
							"==> known loop at " << dr->loopIteration);
					if (!m_qLoopData.isEmpty()) {
						lData = m_qLoopData.top();
						if (updateCovermap) {
							lData->addLoopIteration(&loopCondition,
									dr->loopIteration);
						}
					} else {
						/* TODO error handling */
						QMessageBox::warning(this, "Warning",
								"An error occurred while trying to "
										"get the loop data.");
						ShaderStep (DBG_BH_JUMP_OVER);
						return;
					}
				}
			}
				break;
			}

			if (updateGUI) {
				LoopDialog *lDialog;
				switch (currentRunLevel) {
				case RL_DBG_FRAGMENT_SHADER:
					lDialog = new LoopDialog(lData, this);
					break;
				case RL_DBG_GEOMETRY_SHADER: {
					/* Create list of all watch item boxes */
					QList<ShVarItem*> watchItems;
					if (m_pShVarModel) {
						watchItems = m_pShVarModel->getAllWatchItemPointers();
					}
					lDialog = new LoopDialog(lData, watchItems, m_primitiveMode,
							m_dShResources.geoOutputType, m_pGeometryMap,
							m_pVertexCount, this);
				}
					break;
				case RL_DBG_VERTEX_SHADER: {
					/* Create list of all watch item boxes */
					QList<ShVarItem*> watchItems;
					if (m_pShVarModel) {
						watchItems = m_pShVarModel->getAllWatchItemPointers();
					}
					lDialog = new LoopDialog(lData, watchItems, this);
				}
					break;
				default:
					lDialog = NULL;
					QMessageBox::warning(this, "Warning", "The "
							"current run level does not match the request.");
				}
				if (lDialog) {
					connect(lDialog, SIGNAL(doShaderStep(int, bool, bool)),
							this, SLOT(ShaderStep(int, bool, bool)));
					switch (lDialog->exec()) {
					case LoopDialog::SA_NEXT:
						ShaderStep (DBG_BH_LOOP_NEXT_ITER);
						break;
					case LoopDialog::SA_BREAK:
						ShaderStep (DBG_BH_JUMP_OVER);
						break;
					case LoopDialog::SA_JUMP:
						/* Force update of all changed items */
						updateWatchListData(COVERAGEMAP_GROWN, false);
						ShaderStep(DBG_BH_JUMP_INTO);
						break;
					}
					disconnect(lDialog, 0, 0, 0);
					delete lDialog;
				} else {
					ShaderStep (DBG_BH_JUMP_OVER);
				}
			}
		}
			break;
		default:
			break;
		}

		/* Update GUI shader text windows */
		if (updateGUI) {
			QTextDocument *document = NULL;
			QTextEdit *edit = NULL;
			switch (currentRunLevel) {
			case RL_DBG_VERTEX_SHADER:
				document = teVertexShader->document();
				edit = teVertexShader;
				break;
			case RL_DBG_GEOMETRY_SHADER:
				document = teGeometryShader->document();
				edit = teGeometryShader;
				break;
			case RL_DBG_FRAGMENT_SHADER:
				document = teFragmentShader->document();
				edit = teFragmentShader;
				break;
			default:
				QMessageBox::warning(this, "Warning", "The "
						"current run level does not allow for shader "
						"stepping.");
			}

			/* Mark actual debug position */
			if (document && edit) {
				QTextCharFormat highlight;
				QTextCursor cursor(document);

				cursor.setPosition(0, QTextCursor::MoveAnchor);
				cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor,
						1);
				highlight.setBackground(Qt::white);
				cursor.mergeCharFormat(highlight);

				/* Highlight the actual statement */
				cursor.setPosition(0, QTextCursor::MoveAnchor);
				cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor,
						dr->range.left.line - 1);
				cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor,
						dr->range.left.colum - 1);
				cursor.setPosition(0, QTextCursor::KeepAnchor);
				cursor.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor,
						dr->range.right.line - 1);
				cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor,
						dr->range.right.colum);
				highlight.setBackground(Qt::yellow);
				cursor.mergeCharFormat(highlight);

				/* Ensure the highlighted line is visible */
				QTextCursor cursorVisible = edit->textCursor();
				cursorVisible.setPosition(0, QTextCursor::MoveAnchor);
				cursorVisible.movePosition(QTextCursor::Down,
						QTextCursor::MoveAnchor, MAX(dr->range.left.line-3, 0));
				edit->setTextCursor(cursorVisible);
				edit->ensureCursorVisible();
				cursorVisible.setPosition(0, QTextCursor::KeepAnchor);
				cursorVisible.movePosition(QTextCursor::Down,
						QTextCursor::KeepAnchor, dr->range.right.line + 1);
				edit->setTextCursor(cursorVisible);
				edit->ensureCursorVisible();

				/* Unselect visible cursor */
				QTextCursor cursorSet = edit->textCursor();
				cursorSet.setPosition(0, QTextCursor::MoveAnchor);
				cursorSet.movePosition(QTextCursor::Down,
						QTextCursor::MoveAnchor, dr->range.left.line - 1);
				edit->setTextCursor(cursorSet);
				qApp->processEvents();
			}
		}
	} else {
		/* TODO: error */
	}

	/* TODO free debug result */
}

pcErrorCode MainWindow::recordCall()
{
	resetPerFrameStatistics();

	setGlTraceItemIconType(GlTraceListItem::IT_RECORD);

	pcErrorCode error = pc->recordCall();
	/* TODO: error handling!!!!!! */

	error = pc->callDone();

	delete m_pCurrentCall;
	m_pCurrentCall = NULL;

	/* Error handling */
	setErrorStatus(error);
	if (isErrorCritical(error)) {
		killProgram(1);
		setRunLevel(RL_SETUP);
	} else {
		error = getNextCall();
		setErrorStatus(error);
		if (isErrorCritical(error)) {
			killProgram(1);
			setRunLevel(RL_SETUP);
		} else {
			addGlTraceItem();
		}
	}
	return error;
}

void MainWindow::recordDrawCall()
{
	pc->initRecording();
	if (!strcmp(m_pCurrentCall->getName(), "glBegin")) {
		while (currentRunLevel == RL_DBG_RECORD_DRAWCALL
				&& (!m_pCurrentCall
						|| (m_pCurrentCall
								&& strcmp(m_pCurrentCall->getName(), "glEnd")))) {
			if (recordCall() != PCE_NONE) {
				return;
			}
			qApp->processEvents(QEventLoop::AllEvents);
		}
		if (!m_pCurrentCall || strcmp(m_pCurrentCall->getName(), "glEnd")
				|| recordCall() != PCE_NONE) {
			if (currentRunLevel == RL_DBG_RECORD_DRAWCALL) {
				/* TODO: error handling */
				UT_NOTIFY(LV_WARN, "recordDrawCall: begin without end????");
				return;
			} else {
				/* draw call recording stopped by user interaction */
				pcErrorCode error = pc->insertGlEnd();
				if (error == PCE_NONE) {
					switch (twShader->currentIndex()) {
					case 0:
						error = pc->restoreRenderTarget(
								DBG_TARGET_VERTEX_SHADER);
						break;
					case 1:
						error = pc->restoreRenderTarget(
								DBG_TARGET_GEOMETRY_SHADER);
						break;
					case 2:
						error = pc->restoreRenderTarget(
								DBG_TARGET_FRAGMENT_SHADER);
						break;
					}
				}
				if (error == PCE_NONE) {
					error = pc->restoreActiveShader();
				}
				if (error == PCE_NONE) {
					error = pc->endReplay();
				}
				setErrorStatus(error);
				if (isErrorCritical(error)) {
					killProgram(1);
					setRunLevel(RL_SETUP);
					return;
				}
				setGlTraceItemIconType(GlTraceListItem::IT_ACTUAL);
				return;
			}
		}
		setGlTraceItemIconType(GlTraceListItem::IT_ACTUAL);
	} else {
		if (recordCall() != PCE_NONE) {
			return;
		}
	}
}

void MainWindow::on_tbShaderExecute_clicked()
{
	int type = twShader->currentIndex();
	QString sourceCode;
	char *shaderCode = NULL;
	int debugOptions = EDebugOpIntermediate;
	EShLanguage language = EShLangFragment;
	pcErrorCode error = PCE_NONE;

	if (currentRunLevel == RL_DBG_VERTEX_SHADER
			|| currentRunLevel == RL_DBG_GEOMETRY_SHADER
			|| currentRunLevel == RL_DBG_FRAGMENT_SHADER) {

		/* clean up debug run */
		cleanupDBGShader();

		setRunLevel(RL_DBG_RESTART);
		return;
	}

	if (currentRunLevel == RL_TRACE_EXECUTE_IS_DEBUGABLE) {
		error = pc->saveAndInterruptQueries();
		setErrorStatus(error);
		if (isErrorCritical(error)) {
			killProgram(1);
			setRunLevel(RL_SETUP);
			return;
		}
	}

	/* setup debug render target */
	switch (type) {
	case 0:
		error = pc->setDbgTarget(DBG_TARGET_VERTEX_SHADER, DBG_PFT_KEEP,
				DBG_PFT_KEEP, DBG_PFT_KEEP, DBG_PFT_KEEP);
		break;
	case 1:
		error = pc->setDbgTarget(DBG_TARGET_GEOMETRY_SHADER, DBG_PFT_KEEP,
				DBG_PFT_KEEP, DBG_PFT_KEEP, DBG_PFT_KEEP);
		break;
	case 2:
		error = pc->setDbgTarget(DBG_TARGET_FRAGMENT_SHADER,
				m_pftDialog->alphaTestOption(), m_pftDialog->depthTestOption(),
				m_pftDialog->stencilTestOption(),
				m_pftDialog->blendingOption());
		break;
	}
	setErrorStatus(error);
	if (error != PCE_NONE) {
		if (isErrorCritical(error)) {
			killProgram(1);
			setRunLevel(RL_SETUP);
			return;
		} else {
			setRunLevel(RL_DBG_RESTART);
			return;
		}
	}

	/* record stream, store currently active shader program,
	 * and enter debug state only when shader is executed the first time
	 * and not after restart.
	 */
	if (currentRunLevel == RL_TRACE_EXECUTE_IS_DEBUGABLE) {
		setRunLevel(RL_DBG_RECORD_DRAWCALL);
		/* save active shader */
		error = pc->saveActiveShader();
		setErrorStatus(error);
		if (isErrorCritical(error)) {
			killProgram(1);
			setRunLevel(RL_SETUP);
			return;
		}
		recordDrawCall();
		/* has the user interrupted the recording? */
		if (currentRunLevel != RL_DBG_RECORD_DRAWCALL) {
			return;
		}
	}

	switch (type) {
	case 0: /* Vertex shaders */
		if (m_pShaders[0]) {
			setRunLevel(RL_DBG_VERTEX_SHADER);
			language = EShLangVertex;
		} else {
			return;
		}
		break;
	case 1: /* Geometry shaders */
		if (m_pShaders[1]) {
			language = EShLangGeometry;
			setRunLevel(RL_DBG_GEOMETRY_SHADER);
		} else {
			return;
		}
		break;
	case 2: /* Fragment shaders */
		if (m_pShaders[2]) {
			language = EShLangFragment;
			setRunLevel(RL_DBG_FRAGMENT_SHADER);
		} else {
			return;
		}
		break;
	default:
		return;
	}

	/* start building the parse tree for this shader */
	m_dShCompiler = ShConstructCompiler(language, debugOptions);
	if (m_dShCompiler == 0) {
		setErrorStatus(PCE_UNKNOWN_ERROR);
		killProgram(1);
		setRunLevel(RL_SETUP);
		return;
	}

	shaderCode = m_pShaders[type];

	if (!ShCompile(m_dShCompiler, &shaderCode, 1, EShOptNone, &m_dShResources,
			debugOptions, &m_dShVariableList)) {
		const char *err = ShGetInfoLog(m_dShCompiler);
		Dialog_CompilerError dlgCompilerError(this);
		dlgCompilerError.labelMessage->setText(
				"Your shader seems not to be compliant to the official GLSL1.2 specification and may rely on vendor specific enhancements.<br>If this is not the case, please report this probem to <A HREF=\"mailto:glsldevil@vis.uni-stuttgart.de\">glsldevil@vis.uni-stuttgart.de</A>.");
		dlgCompilerError.setDetailedOutput(err);
		dlgCompilerError.exec();
		cleanupDBGShader();
		setRunLevel(RL_DBG_RESTART);
		return;
	}

	m_pShVarModel = new ShVarModel(&m_dShVariableList, this, qApp);
	connect(m_pShVarModel, SIGNAL(newWatchItem(ShVarItem*)), this,
			SLOT(updateWatchItemData(ShVarItem*)));
	QItemSelectionModel *selectionModel = new QItemSelectionModel(
			m_pShVarModel->getFilterModel(ShVarModel::TV_WATCH_LIST));

	m_pShVarModel->attach(tvShVarAll, ShVarModel::TV_ALL);
	m_pShVarModel->attach(tvShVarBuiltIn, ShVarModel::TV_BUILTIN);
	m_pShVarModel->attach(tvShVarScope, ShVarModel::TV_SCOPE);
	m_pShVarModel->attach(tvShVarUniform, ShVarModel::TV_UNIFORM);
	m_pShVarModel->attach(tvWatchList, ShVarModel::TV_WATCH_LIST);

	tvWatchList->setSelectionModel(selectionModel);

	/* Watchview feedback for selection tracking */
	connect(tvWatchList->selectionModel(),
			SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
			this,
			SLOT(watchSelectionChanged(const QItemSelection &, const QItemSelection &)));

	/* set uniform values */
	m_pShVarModel->setUniformValues(m_serializedUniforms.pData,
			m_serializedUniforms.count);

	ShaderStep (DBG_BH_JUMP_INTO);

	if (currentRunLevel != RL_DBG_VERTEX_SHADER
			&& currentRunLevel != RL_DBG_GEOMETRY_SHADER
			&& currentRunLevel != RL_DBG_FRAGMENT_SHADER) {
		return;
	}

	if (language == EShLangGeometry) {
		delete m_pGeometryMap;
		delete m_pVertexCount;
		m_pGeometryMap = new VertexBox();
		m_pVertexCount = new VertexBox();
		UT_NOTIFY(LV_INFO, "Get GEOMETRY_MAP:");
		if (getDebugVertexData(DBG_CG_GEOMETRY_MAP, NULL, NULL,
				m_pGeometryMap)) {
			/* TODO: build geometry model */
		} else {
			cleanupDBGShader();
			setRunLevel(RL_DBG_RESTART);
			return;
		}
		UT_NOTIFY(LV_INFO, "Get VERTEX_COUNT:");
		if (getDebugVertexData(DBG_CG_VERTEX_COUNT, NULL, NULL,
				m_pVertexCount)) {
			/* TODO: build geometry model */
		} else {
			cleanupDBGShader();
			setRunLevel(RL_DBG_RESTART);
			return;
		}

		//m_pGeoDataModel = new GeoShaderDataModel(m_primitiveMode,
		//			m_dShResources.geoOutputType, m_pGeometryMap, m_pVertexCount);

	}

}

void MainWindow::on_tbShaderReset_clicked()
{
	ShaderStep(DBG_BH_RESET, true);
	ShaderStep (DBG_BH_JUMP_INTO);

	tbShaderStep->setEnabled(true);
	tbShaderStepOver->setEnabled(true);

	resetWatchListData();
}

void MainWindow::on_tbShaderStep_clicked()
{
	ShaderStep (DBG_BH_JUMP_INTO);
}

void MainWindow::on_tbShaderStepOver_clicked()
{
	ShaderStep (DBG_BH_FOLLOW_ELSE);
}

void MainWindow::on_tbShaderFragmentOptions_clicked()
{
	m_pftDialog->show();
}

void MainWindow::on_twShader_currentChanged(int selection)
{
	if (currentRunLevel == RL_DBG_VERTEX_SHADER
			|| currentRunLevel == RL_DBG_GEOMETRY_SHADER
			|| currentRunLevel == RL_DBG_FRAGMENT_SHADER) {
		if ((currentRunLevel == RL_DBG_VERTEX_SHADER && selection == 0)
				|| (currentRunLevel == RL_DBG_GEOMETRY_SHADER && selection == 1)
				|| (currentRunLevel == RL_DBG_FRAGMENT_SHADER && selection == 2)) {
			tbShaderExecute->setEnabled(true);
			tbShaderReset->setEnabled(true);
			tbShaderStepOver->setEnabled(true);
			tbShaderStep->setEnabled(true);
		} else {
			tbShaderExecute->setEnabled(false);
			tbShaderReset->setEnabled(false);
			tbShaderStepOver->setEnabled(false);
			tbShaderStep->setEnabled(false);
			tbShaderFragmentOptions->setEnabled(false);
		}
	} else if ((currentRunLevel == RL_DBG_RESTART
			|| currentRunLevel == RL_TRACE_EXECUTE_IS_DEBUGABLE)
			&& m_bHaveValidShaderCode) {
		switch (twShader->currentIndex()) {
		case 0:
			if (m_pShaders[0] && m_dShResources.transformFeedbackSupported) {
				tbShaderExecute->setEnabled(true);
			} else {
				tbShaderExecute->setEnabled(false);
			}
			tbShaderFragmentOptions->setEnabled(false);
			break;
		case 1:
			if (m_pShaders[1] && m_dShResources.geoShaderSupported
					&& m_dShResources.transformFeedbackSupported) {
				tbShaderExecute->setEnabled(true);
			} else {
				tbShaderExecute->setEnabled(false);
			}
			tbShaderFragmentOptions->setEnabled(false);
			break;
		case 2:
			if (m_pShaders[2] && m_dShResources.framebufferObjectsSupported) {
				tbShaderExecute->setEnabled(true);
				tbShaderFragmentOptions->setEnabled(true);
			} else {
				tbShaderExecute->setEnabled(false);
			}
			break;
		}
	} else {
		tbShaderExecute->setEnabled(false);
	}
}

QModelIndexList MainWindow::cleanupSelectionList(QModelIndexList input)
{
	QModelIndexList output;         // Resulting filtered list.
	QStack<QModelIndex> stack;      // For iterative tree traversal.

	if (!m_pShVarModel) {
		return output;
	}

	//for (int i = 0; i < input.count(); i++) {
	//    ShVarItem *item = m_pShVarModel->getWatchItemPointer(input[i]);

	//    if (item->isSelectable()) {
	//        output << input[i];
	//    }
	//}

	/*
	 * Add directly selected items in reverse order such that getting them
	 * from the stack restores the original order. This is also required
	 * for all following push operations.
	 */
	for (int i = input.count() - 1; i >= 0; i--) {
		stack.push(input[i]);
	}

	while (!stack.isEmpty()) {
		QModelIndex idx = stack.pop();
		ShVarItem *item = this->m_pShVarModel->getWatchItemPointer(idx);

		// Item might be NULL because 'idx' can become invalid during recursion
		// which causes the parent removal to crash.
		if (item != NULL) {
			for (int c = item->childCount() - 1; c >= 0; c--) {
				stack.push(idx.child(c, idx.column()));
			}

			if ((item->childCount() == 0) && item->isSelectable()
					&& !output.contains(idx)) {
				output << idx;
			}
		}
	}

	return output;
}

WatchView* MainWindow::newWatchWindowFragment(QModelIndexList &list)
{
	WatchVector *window = NULL;
	int i;

	for (i = 0; i < list.count(); i++) {
		ShVarItem *item = m_pShVarModel->getWatchItemPointer(list[i]);
		if (item) {
			if (!window) {
				/* Create window */
				window = new WatchVector(workspace);
				connect(window, SIGNAL(destroyed()), this,
						SLOT(watchWindowClosed()));
				connect(window,
						SIGNAL(mouseOverValuesChanged(int, int, const bool *, const QVariant *)),
						this,
						SLOT(setMouseOverValues(int, int, const bool *, const QVariant *)));
				connect(window, SIGNAL(selectionChanged(int, int)), this,
						SLOT(newSelectedPixel(int, int)));
				connect(aZoom, SIGNAL(triggered()), window,
						SLOT(setZoomMode()));
				connect(aSelectPixel, SIGNAL(triggered()), window,
						SLOT(setPickMode()));
				connect(aMinMaxLens, SIGNAL(triggered()), window,
						SLOT(setMinMaxMode()));
				/* initialize mouse mode */
				agWatchControl->setEnabled(true);
				if (agWatchControl->checkedAction() == aZoom) {
					window->setZoomMode();
				} else if (agWatchControl->checkedAction() == aSelectPixel) {
					window->setPickMode();
				} else if (agWatchControl->checkedAction() == aMinMaxLens) {
					window->setMinMaxMode();
				}
				window->setWorkspace(workspace);
				workspace->addWindow(window);
			}
			window->attachFpData(item->getPixelBoxPointer(),
					item->getFullName());
		}
	}
	return window;
}

WatchView* MainWindow::newWatchWindowVertexTable(QModelIndexList &list)
{
	WatchTable *window = NULL;
	int i;

	for (i = 0; i < list.count(); i++) {
		ShVarItem *item = m_pShVarModel->getWatchItemPointer(list[i]);
		if (item) {
			if (!window) {
				/* Create window */
				window = new WatchTable(workspace);
				connect(window, SIGNAL(selectionChanged(int)), this,
						SLOT(newSelectedVertex(int)));
				workspace->addWindow(window);
			}
			window->attachVpData(item->getVertexBoxPointer(),
					item->getFullName());
		}
	}
	return window;
}

WatchView* MainWindow::newWatchWindowGeoDataTree(QModelIndexList &list)
{
	WatchGeoDataTree *window = NULL;
	int i;

	for (i = 0; i < list.count(); i++) {
		ShVarItem *item = m_pShVarModel->getWatchItemPointer(list[i]);
		if (item) {
			if (!window) {
				/* Create window */
				window = new WatchGeoDataTree(m_primitiveMode,
						m_dShResources.geoOutputType, m_pGeometryMap,
						m_pVertexCount, workspace);
				connect(window, SIGNAL(selectionChanged(int)), this,
						SLOT(newSelectedPrimitive(int)));
				workspace->addWindow(window);
			}
			window->attachData(item->getCurrentPointer(),
					item->getVertexBoxPointer(), item->getFullName());
		}
	}
	return window;
}

void MainWindow::on_tbWatchWindow_clicked()
{
	QModelIndexList list = cleanupSelectionList(
			tvWatchList->selectionModel()->selectedRows(0));

	if (m_pShVarModel && !list.isEmpty()) {
		WatchView *window = NULL;
		switch (currentRunLevel) {
		case RL_DBG_GEOMETRY_SHADER:
			window = newWatchWindowGeoDataTree(list);
			break;
		case RL_DBG_VERTEX_SHADER:
			window = newWatchWindowVertexTable(list);
			break;
		case RL_DBG_FRAGMENT_SHADER:
			window = newWatchWindowFragment(list);
			break;
		default:
			UT_NOTIFY(LV_WARN, "invalid runlevel");
		}
		if (window) {
			window->updateView(true);
			window->show();
		} else {
			// TODO: Should this be an error?
			QMessageBox::warning(this, "Warning", "The "
					"watch window could not be created.");
		}
	}
}

void MainWindow::addToWatchWindowVertexTable(WatchView *watchView,
		QModelIndexList &list)
{
	WatchTable *window = static_cast<WatchTable*>(watchView);
	int i;

	for (i = 0; i < list.count(); i++) {
		ShVarItem *item = m_pShVarModel->getWatchItemPointer(list[i]);
		if (item) {
			window->attachVpData(item->getVertexBoxPointer(),
					item->getFullName());
		}
	}
}

void MainWindow::addToWatchWindowFragment(WatchView *watchView,
		QModelIndexList &list)
{
	WatchVector *window = static_cast<WatchVector*>(watchView);
	int i;

	for (i = 0; i < list.count(); i++) {
		ShVarItem *item = m_pShVarModel->getWatchItemPointer(list[i]);
		if (item) {
			window->attachFpData(item->getPixelBoxPointer(),
					item->getFullName());
		}
	}
}

void MainWindow::addToWatchWindowGeoDataTree(WatchView *watchView,
		QModelIndexList &list)
{
	WatchGeoDataTree *window = static_cast<WatchGeoDataTree*>(watchView);
	int i;

	for (i = 0; i < list.count(); i++) {
		ShVarItem *item = m_pShVarModel->getWatchItemPointer(list[i]);
		if (item) {
			window->attachData(item->getCurrentPointer(),
					item->getVertexBoxPointer(), item->getFullName());
		}
	}
}

void MainWindow::on_tbWatchWindowAdd_clicked()
{
	/* TODO: do not use static cast! instead watchView should provide functionality */
	WatchView *window = static_cast<WatchView*>(workspace->activeWindow());
	QModelIndexList list = cleanupSelectionList(
			tvWatchList->selectionModel()->selectedRows(0));

	if (window && m_pShVarModel && !list.isEmpty()) {
		switch (currentRunLevel) {
		case RL_DBG_GEOMETRY_SHADER:
			addToWatchWindowGeoDataTree(window, list);
			break;
		case RL_DBG_VERTEX_SHADER:
			addToWatchWindowVertexTable(window, list);
			break;
		case RL_DBG_FRAGMENT_SHADER:
			addToWatchWindowFragment(window, list);
			break;
		default:
			QMessageBox::critical(this, "Internal Error",
					"on_tbWatchWindowAdd_clicked was called on an invalid "
							"run level.<br>Please report this probem to "
							"<A HREF=\"mailto:glsldevil@vis.uni-stuttgart.de\">"
							"glsldevil@vis.uni-stuttgart.de</A>.",
					QMessageBox::Ok);
		}
		window->updateView(true);
	}
}

void MainWindow::watchWindowClosed()
{
	QWidgetList windowList = workspace->windowList();
	if (windowList.count() == 0) {
		agWatchControl->setEnabled(false);
	}
}

void MainWindow::updateWatchGui(int s)
{
	if (s == 0) {
		tbWatchWindow->setEnabled(false);
		tbWatchWindowAdd->setEnabled(false);
		tbWatchDelete->setEnabled(false);
	} else if (s == 1) {
		tbWatchWindow->setEnabled(true);
		if (workspace->activeWindow()) {
			tbWatchWindowAdd->setEnabled(true);
		} else {
			tbWatchWindowAdd->setEnabled(false);
		}
		tbWatchDelete->setEnabled(true);
	} else {
		tbWatchWindow->setEnabled(true);
		if (workspace->activeWindow()) {
			tbWatchWindowAdd->setEnabled(true);
		} else {
			tbWatchWindowAdd->setEnabled(false);
		}
		tbWatchDelete->setEnabled(true);
	}
}

void MainWindow::on_tbWatchDelete_clicked()
{
	int i;

	if (!tvWatchList)
		return;
	if (!m_pShVarModel)
		return;

	QModelIndexList list;
	do {
		list = tvWatchList->selectionModel()->selectedIndexes();
		if (list.count() != 0) {
			m_pShVarModel->unsetItemWatched(list[0]);
		}
	} while (list.count() != 0);

	updateWatchGui(
			cleanupSelectionList(tvWatchList->selectionModel()->selectedRows()).count());

	/* Now update all windows to update themselves if necessary */
	QWidgetList windowList = workspace->windowList();

	for (i = 0; i < windowList.count(); i++) {
		WatchView *wv = static_cast<WatchView*>(windowList[i]);
		wv->updateView(false);
	}
}

void MainWindow::watchSelectionChanged(const QItemSelection& selected,
		const QItemSelection& deselected)
{
	UNUSED_ARG(selected)
	UNUSED_ARG(deselected)
	updateWatchGui(
			cleanupSelectionList(tvWatchList->selectionModel()->selectedRows()).count());
}

void MainWindow::changedActiveWindow(QWidget *w)
{
	QWidgetList list = workspace->windowList();
	int i;

	for (i = 0; i < list.count(); i++) {
		if (w == list[i]) {
			static_cast<WatchView*>(list[i])->setActive(true);
		} else {
			static_cast<WatchView*>(list[i])->setActive(false);
		}
	}
}

void MainWindow::leaveDBGState()
{
	pcErrorCode error = PCE_NONE;

	switch (currentRunLevel) {
	case RL_DBG_RESTART:
	case RL_DBG_RECORD_DRAWCALL:
		/* restore shader program */
		error = pc->restoreActiveShader();
		setErrorStatus(error);
		if (isErrorCritical(error)) {
			killProgram(1);
			setRunLevel(RL_SETUP);
			return;
		}
		error = pc->restartQueries();
		setErrorStatus(error);
		if (isErrorCritical(error)) {
			killProgram(1);
			setRunLevel(RL_SETUP);
			return;
		}
		error = pc->endReplay();
		setErrorStatus(error);
		if (isErrorCritical(error)) {
			killProgram(1);
			setRunLevel(RL_SETUP);
			return;
		}
		/* TODO: close all windows (obsolete?) */
		delete[] m_pCoverage;
		m_pCoverage = NULL;
		break;
	default:
		break;
	}
}

void MainWindow::cleanupDBGShader()
{
	QList<ShVarItem*> watchItems;
	int i;

	pcErrorCode error = PCE_NONE;

	/* remove debug markers from code display */
	QTextDocument *document = NULL;
	QTextEdit *edit = NULL;
	switch (currentRunLevel) {
	case RL_DBG_VERTEX_SHADER:
		document = teVertexShader->document();
		edit = teVertexShader;
		break;
	case RL_DBG_GEOMETRY_SHADER:
		document = teGeometryShader->document();
		edit = teGeometryShader;
		break;
	case RL_DBG_FRAGMENT_SHADER:
		document = teFragmentShader->document();
		edit = teFragmentShader;
		break;
	}
	if (document && edit) {
		QTextCharFormat highlight;
		QTextCursor cursor(document);

		cursor.setPosition(0, QTextCursor::MoveAnchor);
		cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor, 1);
		highlight.setBackground(Qt::white);
		cursor.mergeCharFormat(highlight);
	}

	lWatchSelectionPos->setText("No Selection");
	m_selectedPixel[0] = -1;
	m_selectedPixel[1] = -1;

	switch (currentRunLevel) {
	case RL_DBG_GEOMETRY_SHADER:
		//delete m_pGeoDataModel;
	case RL_DBG_VERTEX_SHADER:
	case RL_DBG_FRAGMENT_SHADER:
		while (!m_qLoopData.isEmpty()) {
			delete m_qLoopData.pop();
		}

		if (m_pShVarModel) {
			/* free data boxes */
			watchItems = m_pShVarModel->getAllWatchItemPointers();
			for (i = 0; i < watchItems.count(); i++) {
				PixelBox *fb;
				VertexBox *vb;
				if ((fb = watchItems[i]->getPixelBoxPointer())) {
					watchItems[i]->setPixelBoxPointer(NULL);
					delete fb;
				}
				if ((vb = watchItems[i]->getCurrentPointer())) {
					watchItems[i]->setCurrentPointer(NULL);
					delete vb;
				}
				if ((vb = watchItems[i]->getVertexBoxPointer())) {
					watchItems[i]->setVertexBoxPointer(NULL);
					delete vb;
				}
			}

			/* Watchview feedback for selection tracking */
			disconnect(tvWatchList->selectionModel(),
					SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
					this,
					SLOT(watchSelectionChanged(const QItemSelection &, const QItemSelection &)));

			/* reset shader debuging */
			m_pShVarModel->detach(tvShVarAll);
			m_pShVarModel->detach(tvShVarBuiltIn);
			m_pShVarModel->detach(tvShVarScope);
			m_pShVarModel->detach(tvShVarUniform);
			m_pShVarModel->detach(tvWatchList);

			disconnect(m_pShVarModel, SIGNAL(newWatchItem(ShVarItem*)), this,
					0);
			delete m_pShVarModel;
			m_pShVarModel = NULL;
		}

		if (m_dShCompiler) {
			// It is not needed in mesa-glsl
			freeShVariableList(&m_dShVariableList);
			ShDestruct(m_dShCompiler);
			m_dShCompiler = 0;
		}

		UT_NOTIFY(LV_INFO, "restore render target");
		/* restore render target */
		switch (currentRunLevel) {
		case RL_DBG_VERTEX_SHADER:
			error = pc->restoreRenderTarget(DBG_TARGET_VERTEX_SHADER);
			break;
		case RL_DBG_GEOMETRY_SHADER:
			error = pc->restoreRenderTarget(DBG_TARGET_GEOMETRY_SHADER);
			break;
		case RL_DBG_FRAGMENT_SHADER:
			error = pc->restoreRenderTarget(DBG_TARGET_FRAGMENT_SHADER);
			break;
		default:
			error = PCE_NONE;
		}

		setErrorStatus(error);
		if (error != PCE_NONE) {
			if (isErrorCritical(error)) {
				setRunLevel(RL_SETUP);
				UT_NOTIFY(LV_ERROR, getErrorDescription(error));
				killProgram(1);
				return;
			}
			UT_NOTIFY(LV_WARN, getErrorDescription(error));
			return;
		}
		break;
	default:
		break;
	}
}

void MainWindow::setRunLevel(int rl)
{
	QString title = QString(MAIN_WINDOW_TITLE);
	UT_NOTIFY(LV_INFO,
			"new level: " << rl << " " << (m_pCurrentCall ? m_pCurrentCall->getName() : ""));

	switch (rl) {
	case RL_INIT:  // Program start
		currentRunLevel = RL_INIT;
		this->setWindowTitle(title);
		aOpen->setEnabled(true);
#ifdef _WIN32
		//aAttach->setEnabled(true);
		// TODO
#endif /* _WIN32 */
		tbBVCaptureAutomatic->setEnabled(false);
		tbBVCaptureAutomatic->setChecked(false);
		tbBVCapture->setEnabled(false);
		tbBVSave->setEnabled(false);
		lBVLabel->setPixmap(QPixmap());
		tbExecute->setEnabled(false);
		tbExecute->setIcon(
				QIcon(
						QString::fromUtf8(
								":/icons/icons/gltrace-execute_32.png")));
		tbStep->setEnabled(false);
		tbSkip->setEnabled(false);
		tbEdit->setEnabled(false);
		tbJumpToDrawCall->setEnabled(false);
		tbJumpToShader->setEnabled(false);
		tbJumpToUserDef->setEnabled(false);
		tbRun->setEnabled(false);
		tbPause->setEnabled(false);
		tbToggleGuiUpdate->setEnabled(false);
		tbToggleNoTrace->setEnabled(false);
		tbToggleHaltOnError->setEnabled(false);
		tbGlTraceSettings->setEnabled(false);
		tbSave->setEnabled(false);
		tbShaderExecute->setEnabled(false);
		tbShaderExecute->setIcon(
				QIcon(
						QString::fromUtf8(
								":/icons/icons/shader-execute_32.png")));
		tbShaderReset->setEnabled(false);
		tbShaderStep->setEnabled(false);
		tbShaderStepOver->setEnabled(false);
		tbShaderFragmentOptions->setEnabled(false);
		twShader->setTabIcon(0, QIcon());
		twShader->setTabIcon(1, QIcon());
		twShader->setTabIcon(2, QIcon());
		updateWatchGui(0);
		setShaderCodeText(NULL);
		m_bHaveValidShaderCode = false;
		setGuiUpdates(true);
		break;
	case RL_SETUP:  // User has setup parameters for debugging
		currentRunLevel = RL_SETUP;
		title.append(" - ");
		title.append(dbgProgArgs[0]);
		this->setWindowTitle(title);
		aOpen->setEnabled(true);
#ifdef _WIN32
		//aAttach->setEnabled(true);
		// TODO
#endif /* _WIN32 */
		tbBVCapture->setEnabled(false);
		tbBVCaptureAutomatic->setEnabled(false);
		tbBVCaptureAutomatic->setChecked(false);
		tbBVSave->setEnabled(false);
		lBVLabel->setPixmap(QPixmap());
		tbExecute->setEnabled(true);
		tbExecute->setIcon(
				QIcon(
						QString::fromUtf8(
								":/icons/icons/gltrace-execute_32.png")));
		tbStep->setEnabled(false);
		tbSkip->setEnabled(false);
		tbEdit->setEnabled(false);
		tbJumpToDrawCall->setEnabled(false);
		tbJumpToShader->setEnabled(false);
		tbJumpToUserDef->setEnabled(false);
		tbRun->setEnabled(false);
		tbPause->setEnabled(false);
		tbToggleGuiUpdate->setEnabled(false);
		tbToggleNoTrace->setEnabled(false);
		tbShaderExecute->setEnabled(false);
		tbToggleHaltOnError->setEnabled(false);
		tbGlTraceSettings->setEnabled(true);
		tbSave->setEnabled(true);
		tbShaderExecute->setIcon(
				QIcon(
						QString::fromUtf8(
								":/icons/icons/shader-execute_32.png")));
		tbShaderReset->setEnabled(false);
		tbShaderStep->setEnabled(false);
		tbShaderStepOver->setEnabled(false);
		tbShaderFragmentOptions->setEnabled(false);
		twShader->setTabIcon(0, QIcon());
		twShader->setTabIcon(1, QIcon());
		twShader->setTabIcon(2, QIcon());
		updateWatchGui(0);
		setShaderCodeText(NULL);
		m_bHaveValidShaderCode = false;
		setGuiUpdates(true);
		break;
	case RL_TRACE_EXECUTE:  // Trace is running in step mode
		/* choose sub-level */
		if (m_pCurrentCall && m_pCurrentCall->isDebuggable(&m_primitiveMode)
				&& m_bHaveValidShaderCode) {
			setRunLevel(RL_TRACE_EXECUTE_IS_DEBUGABLE);
		} else {
			setRunLevel(RL_TRACE_EXECUTE_NO_DEBUGABLE);
		}
		break;
	case RL_TRACE_EXECUTE_NO_DEBUGABLE:  // sub-level for non debugable calls
		currentRunLevel = RL_TRACE_EXECUTE_NO_DEBUGABLE;
		title.append(" - ");
		title.append(dbgProgArgs[0]);
		this->setWindowTitle(title);
		aOpen->setEnabled(true);
#ifdef _WIN32
		//aAttach->setEnabled(true);
		// TODO
#endif /* _WIN32 */
		if (!tbBVCaptureAutomatic->isChecked()) {
			tbBVCapture->setEnabled(true);
		}
		tbBVCaptureAutomatic->setEnabled(true);
		tbExecute->setEnabled(true);
		tbExecute->setIcon(
				QIcon(
						QString::fromUtf8(
								":/icons/icons/face-devil-green-grin_32.png")));
		tbStep->setEnabled(true);
		tbSkip->setEnabled(true);
		if (m_pCurrentCall && m_pCurrentCall->isEditable()) {
			tbEdit->setEnabled(true);
		} else {
			tbEdit->setEnabled(false);
		}
		tbJumpToDrawCall->setEnabled(true);
		tbJumpToShader->setEnabled(true);
		tbJumpToUserDef->setEnabled(true);
		tbRun->setEnabled(true);
		tbPause->setEnabled(false);
		tbToggleGuiUpdate->setEnabled(true);
		tbToggleNoTrace->setEnabled(true);
		tbToggleHaltOnError->setEnabled(true);
		tbGlTraceSettings->setEnabled(true);
		tbSave->setEnabled(true);
		tbShaderExecute->setEnabled(false);
		tbShaderExecute->setIcon(
				QIcon(
						QString::fromUtf8(
								":/icons/icons/shader-execute_32.png")));
		tbShaderReset->setEnabled(false);
		tbShaderStep->setEnabled(false);
		tbShaderStepOver->setEnabled(false);
		tbShaderFragmentOptions->setEnabled(false);
		twShader->setTabIcon(0, QIcon());
		twShader->setTabIcon(1, QIcon());
		twShader->setTabIcon(2, QIcon());
		updateWatchGui(0);
		setGuiUpdates(true);
		break;
	case RL_TRACE_EXECUTE_IS_DEBUGABLE:  // sub-level for debugable calls
		currentRunLevel = RL_TRACE_EXECUTE_IS_DEBUGABLE;
		title.append(" - ");
		title.append(dbgProgArgs[0]);
		this->setWindowTitle(title);
		aOpen->setEnabled(true);
#ifdef _WIN32
		//aAttach->setEnabled(true);
		// TODO
#endif /* _WIN32 */
		if (!tbBVCaptureAutomatic->isChecked()) {
			tbBVCapture->setEnabled(true);
		}
		tbBVCaptureAutomatic->setEnabled(true);
		tbExecute->setEnabled(true);
		tbExecute->setIcon(
				QIcon(
						QString::fromUtf8(
								":/icons/icons/face-devil-green-grin_32.png")));
		tbStep->setEnabled(true);
		tbSkip->setEnabled(true);
		if (m_pCurrentCall && m_pCurrentCall->isEditable()) {
			tbEdit->setEnabled(true);
		} else {
			tbEdit->setEnabled(false);
		}
		tbJumpToDrawCall->setEnabled(true);
		tbJumpToShader->setEnabled(true);
		tbJumpToUserDef->setEnabled(true);
		tbRun->setEnabled(true);
		tbPause->setEnabled(false);
		tbToggleGuiUpdate->setEnabled(true);
		tbToggleNoTrace->setEnabled(true);
		tbToggleHaltOnError->setEnabled(true);
		tbGlTraceSettings->setEnabled(true);
		tbSave->setEnabled(true);
		if (m_bHaveValidShaderCode) {
			switch (twShader->currentIndex()) {
			case 0:
				if (m_pShaders[0]
						&& m_dShResources.transformFeedbackSupported) {
					tbShaderExecute->setEnabled(true);
				} else {
					tbShaderExecute->setEnabled(false);
				}
				tbShaderFragmentOptions->setEnabled(false);
				break;
			case 1:
				if (m_pShaders[1] && m_dShResources.geoShaderSupported
						&& m_dShResources.transformFeedbackSupported) {
					tbShaderExecute->setEnabled(true);
				} else {
					tbShaderExecute->setEnabled(false);
				}
				tbShaderFragmentOptions->setEnabled(false);
				break;
			case 2:
				if (m_pShaders[2]
						&& m_dShResources.framebufferObjectsSupported) {
					tbShaderExecute->setEnabled(true);
					tbShaderFragmentOptions->setEnabled(true);
				} else {
					tbShaderExecute->setEnabled(false);
					tbShaderFragmentOptions->setEnabled(false);
				}
				break;
			}
		} else {
			tbShaderExecute->setEnabled(false);
			tbShaderFragmentOptions->setEnabled(false);
		}
		tbShaderExecute->setIcon(
				QIcon(
						QString::fromUtf8(
								":/icons/icons/shader-execute_32.png")));
		tbShaderReset->setEnabled(false);
		tbShaderStep->setEnabled(false);
		tbShaderStepOver->setEnabled(false);
		twShader->setTabIcon(0, QIcon());
		twShader->setTabIcon(1, QIcon());
		twShader->setTabIcon(2, QIcon());
		updateWatchGui(0);
		setGuiUpdates(true);
		break;
	case RL_TRACE_EXECUTE_RUN:
		currentRunLevel = RL_TRACE_EXECUTE_RUN;
		title.append(" - ");
		title.append(dbgProgArgs[0]);
		this->setWindowTitle(title);
		aOpen->setEnabled(false);
		aAttach->setEnabled(false);
		if (!tbBVCaptureAutomatic->isChecked()) {
			tbBVCapture->setEnabled(true);
		}
		tbBVCaptureAutomatic->setEnabled(true);
		tbExecute->setEnabled(true);
		tbExecute->setIcon(
				QIcon(
						QString::fromUtf8(
								":/icons/icons/face-devil-green-grin_32.png")));
		tbStep->setEnabled(false);
		tbSkip->setEnabled(false);
		tbEdit->setEnabled(false);
		tbJumpToDrawCall->setEnabled(false);
		tbJumpToShader->setEnabled(false);
		tbJumpToUserDef->setEnabled(false);
		tbRun->setEnabled(false);
		tbPause->setEnabled(true);
		if (tbToggleNoTrace->isChecked()) {
			tbToggleGuiUpdate->setEnabled(false);
		} else {
			tbToggleGuiUpdate->setEnabled(true);
		}
		tbToggleNoTrace->setEnabled(false);
		tbToggleHaltOnError->setEnabled(false);
		tbGlTraceSettings->setEnabled(false);
		tbSave->setEnabled(false);
		tbShaderExecute->setEnabled(false);
		tbShaderExecute->setIcon(
				QIcon(
						QString::fromUtf8(
								":/icons/icons/shader-execute_32.png")));
		tbShaderReset->setEnabled(false);
		tbShaderStep->setEnabled(false);
		tbShaderStepOver->setEnabled(false);
		tbShaderFragmentOptions->setEnabled(false);
		twShader->setTabIcon(0, QIcon());
		twShader->setTabIcon(1, QIcon());
		twShader->setTabIcon(2, QIcon());
		updateWatchGui(0);
		/* update handling */
		if (tbToggleGuiUpdate->isChecked()) {
			setGuiUpdates(false);
		} else {
			setGuiUpdates(true);
		}
		break;
	case RL_DBG_RECORD_DRAWCALL:
		currentRunLevel = RL_DBG_RECORD_DRAWCALL;
		title.append(" - ");
		title.append(dbgProgArgs[0]);
		this->setWindowTitle(title);
		aOpen->setEnabled(false);
		aAttach->setEnabled(false);
		if (!tbBVCaptureAutomatic->isChecked()) {
			tbBVCapture->setEnabled(true);
		}
		tbBVCaptureAutomatic->setEnabled(true);
		tbExecute->setEnabled(true);
		tbExecute->setIcon(
				QIcon(
						QString::fromUtf8(
								":/icons/icons/face-devil-green-grin_32.png")));
		tbStep->setEnabled(false);
		tbSkip->setEnabled(false);
		tbEdit->setEnabled(false);
		tbJumpToDrawCall->setEnabled(false);
		tbJumpToShader->setEnabled(false);
		tbJumpToUserDef->setEnabled(false);
		tbRun->setEnabled(false);
		tbPause->setEnabled(true);
		tbToggleGuiUpdate->setEnabled(true);
		tbToggleNoTrace->setEnabled(false);
		tbToggleHaltOnError->setEnabled(false);
		tbSave->setEnabled(false);
		tbGlTraceSettings->setEnabled(false);
		tbShaderExecute->setEnabled(false);
		tbShaderExecute->setIcon(
				QIcon(
						QString::fromUtf8(
								":/icons/icons/shader-execute_32.png")));
		tbShaderReset->setEnabled(false);
		tbShaderStep->setEnabled(false);
		tbShaderStepOver->setEnabled(false);
		tbShaderFragmentOptions->setEnabled(false);
		twShader->setTabIcon(0, QIcon());
		twShader->setTabIcon(1, QIcon());
		twShader->setTabIcon(2, QIcon());
		updateWatchGui(0);
		setGuiUpdates(true);
		break;
	case RL_DBG_VERTEX_SHADER:
	case RL_DBG_GEOMETRY_SHADER:
	case RL_DBG_FRAGMENT_SHADER:
		currentRunLevel = rl;
		title.append(" - ");
		title.append(dbgProgArgs[0]);
		this->setWindowTitle(title);
		aOpen->setEnabled(true);
#ifdef _WIN32
		//aAttach->setEnabled(true);
		// TODO
#endif /* _WIN32 */
		if (!tbBVCaptureAutomatic->isChecked()) {
			tbBVCapture->setEnabled(true);
		}
		tbBVCaptureAutomatic->setEnabled(true);
		tbExecute->setEnabled(true);
		tbExecute->setIcon(
				QIcon(
						QString::fromUtf8(
								":/icons/icons/face-devil-green-grin_32.png")));
		tbStep->setEnabled(false);
		tbSkip->setEnabled(false);
		tbEdit->setEnabled(false);
		tbJumpToDrawCall->setEnabled(false);
		tbJumpToShader->setEnabled(false);
		tbJumpToUserDef->setEnabled(false);
		tbRun->setEnabled(false);
		tbPause->setEnabled(false);
		tbToggleGuiUpdate->setEnabled(false);
		tbToggleNoTrace->setEnabled(false);
		tbToggleHaltOnError->setEnabled(false);
		tbGlTraceSettings->setEnabled(false);
		tbSave->setEnabled(true);

		if (m_bHaveValidShaderCode) {
			switch (twShader->currentIndex()) {
			case 0:
				if (m_pShaders[0]
						&& m_dShResources.transformFeedbackSupported) {
					tbShaderExecute->setEnabled(true);
				} else {
					tbShaderExecute->setEnabled(false);
				}
				break;
			case 1:
				if (m_pShaders[1] && m_dShResources.geoShaderSupported
						&& m_dShResources.transformFeedbackSupported) {
					tbShaderExecute->setEnabled(true);
				} else {
					tbShaderExecute->setEnabled(false);
				}
				break;
			case 2:
				if (m_pShaders[2]
						&& m_dShResources.framebufferObjectsSupported) {
					tbShaderExecute->setEnabled(true);
				} else {
					tbShaderExecute->setEnabled(false);
				}
				break;
			}
		} else {
			tbShaderExecute->setEnabled(false);
		}
		tbShaderExecute->setIcon(
				QIcon(
						QString::fromUtf8(
								":/icons/icons/face-devil-grin_32.png")));
		tbShaderReset->setEnabled(true);
		tbShaderStep->setEnabled(true);
		tbShaderStepOver->setEnabled(true);
		tbShaderFragmentOptions->setEnabled(false);
		twShader->setTabIcon(0, QIcon());
		twShader->setTabIcon(1, QIcon());
		twShader->setTabIcon(2, QIcon());
		twShader->setTabIcon(rl - RL_DBG_VERTEX_SHADER,
				QIcon(
						QString::fromUtf8(
								":/icons/icons/shader-execute_32.png")));
		updateWatchGui(0);
		setGuiUpdates(true);
		break;
	case RL_DBG_RESTART:
		currentRunLevel = rl;
		title.append(" - ");
		title.append(dbgProgArgs[0]);
		this->setWindowTitle(title);
		aOpen->setEnabled(true);
#ifdef _WIN32
		//aAttach->setEnabled(true);
		// TODO
#endif /* _WIN32 */
		if (!tbBVCaptureAutomatic->isChecked()) {
			tbBVCapture->setEnabled(true);
		}
		tbBVCaptureAutomatic->setEnabled(true);
		tbExecute->setEnabled(true);
		tbExecute->setIcon(
				QIcon(
						QString::fromUtf8(
								":/icons/icons/face-devil-green-grin_32.png")));
		tbStep->setEnabled(true);
		tbSkip->setEnabled(true);
		if (m_pCurrentCall && m_pCurrentCall->isEditable()) {
			tbEdit->setEnabled(true);
		} else {
			tbEdit->setEnabled(false);
		}
		tbJumpToDrawCall->setEnabled(true);
		tbJumpToShader->setEnabled(true);
		tbJumpToUserDef->setEnabled(true);
		tbRun->setEnabled(true);
		tbPause->setEnabled(false);
		tbToggleGuiUpdate->setEnabled(true);
		tbToggleNoTrace->setEnabled(true);
		tbToggleHaltOnError->setEnabled(true);
		tbGlTraceSettings->setEnabled(true);
		tbSave->setEnabled(true);

		if (m_bHaveValidShaderCode) {
			switch (twShader->currentIndex()) {
			case 0:
				if (m_pShaders[0]
						&& m_dShResources.transformFeedbackSupported) {
					tbShaderExecute->setEnabled(true);
				} else {
					tbShaderExecute->setEnabled(false);
				}
				tbShaderFragmentOptions->setEnabled(false);
				break;
			case 1:
				if (m_pShaders[1] && m_dShResources.geoShaderSupported
						&& m_dShResources.transformFeedbackSupported) {
					tbShaderExecute->setEnabled(true);
				} else {
					tbShaderExecute->setEnabled(false);
				}
				tbShaderFragmentOptions->setEnabled(false);
				break;
			case 2:
				if (m_pShaders[2]
						&& m_dShResources.framebufferObjectsSupported) {
					tbShaderExecute->setEnabled(true);
					tbShaderFragmentOptions->setEnabled(true);
				} else {
					tbShaderExecute->setEnabled(false);
					tbShaderFragmentOptions->setEnabled(false);
				}
				break;
			}
		} else {
			tbShaderExecute->setEnabled(false);
		}
		tbShaderExecute->setIcon(
				QIcon(
						QString::fromUtf8(
								":/icons/icons/shader-execute_32.png")));
		tbShaderReset->setEnabled(false);
		tbShaderStep->setEnabled(false);
		tbShaderStepOver->setEnabled(false);
		twShader->setTabIcon(0, QIcon());
		twShader->setTabIcon(1, QIcon());
		twShader->setTabIcon(2, QIcon());
		updateWatchGui(0);
		setGuiUpdates(true);
		break;
	}
}

void MainWindow::setErrorStatus(pcErrorCode error)
{
	QPalette palette;

	switch (error) {
	/* no error */
	case PCE_NONE:
		lSBErrorIcon->setVisible(false);
		lSBErrorText->setVisible(false);
		statusbar->showMessage("");
		break;
		/* gl errors and other non-critical errors */
	case PCE_GL_INVALID_ENUM:
	case PCE_GL_INVALID_VALUE:
	case PCE_GL_INVALID_OPERATION:
	case PCE_GL_STACK_OVERFLOW:
	case PCE_GL_STACK_UNDERFLOW:
	case PCE_GL_OUT_OF_MEMORY:
	case PCE_GL_TABLE_TOO_LARGE:
	case PCE_DBG_READBACK_NOT_ALLOWED:
		lSBErrorIcon->setVisible(true);
		lSBErrorIcon->setPixmap(
				QPixmap(
						QString::fromUtf8(
								":/icons/icons/dialog-error-green.png")));
		lSBErrorText->setVisible(true);
		palette = lSBErrorText->palette();
		palette.setColor(QPalette::WindowText, Qt::green);
		lSBErrorText->setPalette(palette);
		lSBErrorText->setText(getErrorInfo(error));
		statusbar->showMessage(getErrorDescription(error));
		addGlTraceWarningItem(getErrorDescription(error));
		break;
		/* all other errors are considered critical errors */
	default:
		lSBErrorIcon->setVisible(true);
		lSBErrorIcon->setPixmap(
				QPixmap(
						QString::fromUtf8(
								":/icons/icons/dialog-error-blue_32.png")));
		lSBErrorText->setVisible(true);
		palette = lSBErrorText->palette();
		palette.setColor(QPalette::WindowText, Qt::blue);
		lSBErrorText->setPalette(palette);
		lSBErrorText->setText(getErrorInfo(error));
		statusbar->showMessage(getErrorDescription(error));
		addGlTraceErrorItem(getErrorDescription(error));
		break;
	}
}

void MainWindow::setStatusBarText(QString text)
{
	statusbar->showMessage(text);
}

void MainWindow::setMouseOverValues(int x, int y, const bool *active,
		const QVariant *values)
{
	if (x < 0 || y < 0) {
		lSBCurrentPosition->clear();
		lSBCurrentValueR->clear();
		lSBCurrentValueG->clear();
		lSBCurrentValueB->clear();
	} else {
		lSBCurrentPosition->setText(
				QString::number(x) + "," + QString::number(y));
		if (active[0]) {
			lSBCurrentValueR->setText(values[0].toString());
		} else {
			lSBCurrentValueR->clear();
		}
		if (active[1]) {
			lSBCurrentValueG->setText(values[1].toString());
		} else {
			lSBCurrentValueG->clear();
		}
		if (active[2]) {
			lSBCurrentValueB->setText(values[2].toString());
		} else {
			lSBCurrentValueB->clear();
		}
	}
}

void MainWindow::newSelectedPixel(int x, int y)
{
	m_selectedPixel[0] = x;
	m_selectedPixel[1] = y;
	lWatchSelectionPos->setText(
			"Pixel " + QString::number(x) + ", " + QString::number(y));
	if (m_pShVarModel && m_selectedPixel[0] >= 0 && m_selectedPixel[1] >= 0) {
		m_pShVarModel->setCurrentValues(m_selectedPixel[0], m_selectedPixel[1]);
	}
}

void MainWindow::newSelectedVertex(int n)
{
	m_selectedPixel[0] = n;
	m_selectedPixel[1] = -1;
	if (m_pShVarModel && m_selectedPixel[0] >= 0) {
		lWatchSelectionPos->setText("Vertex " + QString::number(n));
		m_pShVarModel->setCurrentValues(m_selectedPixel[0]);
	}
}

void MainWindow::newSelectedPrimitive(int dataIdx)
{
	m_selectedPixel[0] = dataIdx;
	if (m_pShVarModel && m_selectedPixel[0] >= 0) {
		lWatchSelectionPos->setText("Primitive " + QString::number(dataIdx));
		m_pShVarModel->setCurrentValues(m_selectedPixel[0]);
	}
}
void MainWindow::resetPerFrameStatistics(void)
{
	if (m_pCurrentCall && m_pCurrentCall->isFrameEnd()) {
		m_pGlCallPfst->resetStatistic();
		m_pGlExtPfst->resetStatistic();
		m_pGlxCallPfst->resetStatistic();
		m_pGlxExtPfst->resetStatistic();
		m_pWglCallPfst->resetStatistic();
		m_pWglExtPfst->resetStatistic();
	}
}

void MainWindow::resetAllStatistics(void)
{
	m_pGlCallSt->resetStatistic();
	m_pGlExtSt->resetStatistic();
	m_pGlCallPfst->resetStatistic();
	m_pGlExtPfst->resetStatistic();
	m_pGlxCallSt->resetStatistic();
	m_pGlxExtSt->resetStatistic();
	m_pGlxCallPfst->resetStatistic();
	m_pGlxExtPfst->resetStatistic();
	m_pWglCallSt->resetStatistic();
	m_pWglExtSt->resetStatistic();
	m_pWglCallPfst->resetStatistic();
	m_pWglExtPfst->resetStatistic();
}

bool MainWindow::loadMruProgram(QString& outProgram, QString& outArguments,
		QString& outWorkDir)
{
	QSettings settings;
	outProgram = settings.value("MRU/Program", "").toString();
	outArguments = settings.value("MRU/Arguments", "").toString();
	outWorkDir = settings.value("MRU/WorkDir", "").toString();
	return true;
}

bool MainWindow::saveMruProgram(const QString& program,
		const QString& arguments, const QString& workDir)
{
	QSettings settings;
	settings.setValue("MRU/Program", program);
	settings.setValue("MRU/Arguments", arguments);
	settings.setValue("MRU/WorkDir", workDir);
	return true;
}

