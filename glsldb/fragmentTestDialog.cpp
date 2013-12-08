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

#include "fragmentTestDialog.qt.h"
#include "debuglib.h"

static int mapPFTToIndex(int pftOption)
{
	switch (pftOption) {
	case DBG_PFT_FORCE_DISABLED:
		return 1;
	case DBG_PFT_FORCE_ENABLED:
		return 2;
	case DBG_PFT_KEEP:
	default:
		return 0;
	}
}

static int mapIndexToPFT(int index)
{
	switch (index) {
	case 1:
		return DBG_PFT_FORCE_DISABLED;
	case 2:
		return DBG_PFT_FORCE_ENABLED;
	case 0:
	default:
		return DBG_PFT_KEEP;
	}
}

FragmentTestDialog::FragmentTestDialog(QWidget *parent) :
		QDialog(parent)
{
	setupUi(this);
	connect(pbDefaults, SIGNAL(clicked()), this, SLOT(setDefaults()));
	connect(pbReset, SIGNAL(clicked()), this, SLOT(resetSettings()));
	connect(pbOK, SIGNAL(clicked()), this, SLOT(apply()));
	connect(pbCancel, SIGNAL(clicked()), this, SLOT(cancel()));
	leDepthValue->setValidator(new QDoubleValidator(leDepthValue));
	leAlphaValue->setValidator(new QDoubleValidator(leAlphaValue));
	setDefaults();
}

void FragmentTestDialog::setDefaults()
{
	m_alphaTest = DBG_PFT_KEEP;
	m_depthTest = DBG_PFT_KEEP;
	m_stencilTest = DBG_PFT_KEEP;
	m_blending = DBG_PFT_FORCE_DISABLED;

	m_copyAlpha = true;
	m_copyDepth = true;
	m_copyStencil = true;

	m_alphaValue = 0.0;
	m_depthValue = 0.0;
	m_stencilValue = 0;

	resetSettings();
}

void FragmentTestDialog::resetSettings()
{
	cbAlphaTest->setCurrentIndex(mapPFTToIndex(m_alphaTest));
	cbDepthTest->setCurrentIndex(mapPFTToIndex(m_depthTest));
	cbStencilTest->setCurrentIndex(mapPFTToIndex(m_stencilTest));
	cbBlending->setCurrentIndex(mapPFTToIndex(m_blending));

	cbAlphaCopy->setChecked(m_copyAlpha);
	cbDepthCopy->setChecked(m_copyDepth);
	cbStencilCopy->setChecked(m_copyStencil);

	leAlphaValue->setText(QString::number(m_alphaValue));
	leDepthValue->setText(QString::number(m_depthValue));
	leStencilValue->setText(QString::number(m_stencilValue, 16));
}

void FragmentTestDialog::apply()
{
	m_alphaTest = mapIndexToPFT(cbAlphaTest->currentIndex());
	m_depthTest = mapIndexToPFT(cbDepthTest->currentIndex());
	m_stencilTest = mapIndexToPFT(cbStencilTest->currentIndex());
	m_blending = mapIndexToPFT(cbBlending->currentIndex());

	m_copyAlpha = cbAlphaCopy->isChecked();
	m_copyDepth = cbDepthCopy->isChecked();
	m_copyStencil = cbStencilCopy->isChecked();

	m_alphaValue = leAlphaValue->text().toFloat();
	m_depthValue = leDepthValue->text().toFloat();
	m_stencilValue = leStencilValue->text().toInt();
	accept();
}

void FragmentTestDialog::cancel()
{
	reject();
}

