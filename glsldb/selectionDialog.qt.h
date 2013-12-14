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

#ifndef _SELECTIN_DIALOG_QT_H_
#define _SELECTIN_DIALOG_QT_H_

#include "ui_selectionDialog.h"
#include <QtGui/QLabel>
#include <QtGui/QWidget>
#include <QtGui/QScrollArea>
#include <QtCore/QList>

#include "pixelBox.qt.h"
#include "vertexBox.qt.h"
#include "shVarModel.qt.h"

class SelectionDialog: public QDialog, public Ui::dSelection {
Q_OBJECT

public:
	SelectionDialog(PixelBoxFloat *fb, bool elseBranch, QWidget *parent = 0);
	SelectionDialog(VertexBox *vbCondition, QList<ShVarItem*> &watchItems,
			int inPrimitiveType, int outPrimitiveType, VertexBox *primitiveMap,
			VertexBox *vertexCountMap, bool elseBranch, QWidget *parent);
	SelectionDialog(VertexBox *vbCondition, QList<ShVarItem*> &watchItems,
			bool elseBranch, QWidget *parent);

	typedef enum {
		SB_IF = 0,
		SB_SKIP,
		SB_ELSE
	} selectedBranch;

	int exec();

protected:
	void displayStatistics(void);

private slots:
	/* auto-connect */
	void on_pbSkip_clicked();
	void on_pbIf_clicked();
	void on_pbElse_clicked();

private:
	bool m_bElseBranch;
	int m_nTotal;
	int m_nActive;
	int m_nIf;
	int m_nElse;
};

#endif

