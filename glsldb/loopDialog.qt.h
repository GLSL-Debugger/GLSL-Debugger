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

#ifndef _LOOP_DIALOG_QT_H_
#define _LOOP_DIALOG_QT_H_

#include "ui_loopDialog.h"
#include "loopData.qt.h"
#include "curveView.qt.h"
#include <QtGui/QLabel>
#include <QtGui/QWidget>
#include <QtGui/QScrollArea>
#include <QtGui/QTableView>
#include <QtGui/QTreeView>
#include <QtCore/QList>

#include "shVarModel.qt.h"

class LoopDialog : public QDialog, public Ui::dLoop {
    Q_OBJECT

public:
    LoopDialog(LoopData *data, QWidget *parent = 0);

    LoopDialog(LoopData *data, QList<ShVarItem*> &watchItems,
               int inPrimitiveType, int outPrimitiveType,
               VertexBox *primitiveMap,
               VertexBox *vertexCountMap,
               QWidget *parent = 0);

    LoopDialog(LoopData *data,
               QList<ShVarItem*> &watchItems,
               QWidget *parent = 0);

    typedef enum {
        SA_NEXT = 0,
        SA_BREAK,
        SA_JUMP
    } selectedAction;

    int exec();

signals:
    void doShaderStep(int, bool, bool);

private slots:
    /* auto-connect */
    void on_pbBreak_clicked();
    void on_pbStatistics_clicked();
    void on_pbJump_clicked();
    void on_pbNext_clicked();
    void on_pbStopProgress_clicked();
    void on_pbStopJump_clicked();

    void on_cbActive_stateChanged(int);
    void on_cbOut_stateChanged(int);
    void on_cbDone_stateChanged(int);

    /* self connect */
    void reorganizeLoopTable(const QModelIndex&, int, int);
    
private:
    void setupGui(void);

    void updateIterationStatistics();
    
    QScrollArea *m_qScrollArea;
    QLabel      *m_qLabel;

    QTableView  *m_qTableView;
    QTreeView   *m_qTreeView;
    
    CurveView   *m_qCurves;

    LoopData    *m_pData;
    int          m_nActive;
    bool         m_bProcessStopped;
};

#endif

