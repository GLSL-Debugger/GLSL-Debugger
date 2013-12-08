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

#include "compilerErrorDialog.qt.h"

Dialog_CompilerError::Dialog_CompilerError(QWidget *parent) :
		QDialog(parent)
{
	this->setupUi(this);

	/*
	 * Initialise the most expensive dialog of all times. VIS is wasting
	 * taxpayer's money(tm) ...
	 */
	this->details = new Widget_CompilerError(this);

	QGridLayout *layout = dynamic_cast<QGridLayout *>(this->layout());
	layout->setSizeConstraint(QLayout::SetFixedSize);
	layout->addWidget(this->details, 2, 0);
	connect(btnDetails, SIGNAL(clicked()), this, SLOT(toggleDetails()));

	this->details->hide();
}

Dialog_CompilerError::~Dialog_CompilerError(void)
{
	delete this->details;
}

void Dialog_CompilerError::setDetailedOutput(const char *text)
{
	this->details->textBoxCompilerOutput->setText(text);
}

void Dialog_CompilerError::toggleDetails(void)
{
	if (this->details->isHidden()) {
		this->details->show();
		this->btnDetails->setText("Hide Details");
	} else {
		this->details->hide();
		this->btnDetails->setText("Show Details");
	}
}

