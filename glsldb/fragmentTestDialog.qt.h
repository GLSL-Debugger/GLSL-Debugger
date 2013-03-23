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

#ifndef FRAGMENT_TEST_DIALOG_H
#define FRAGMENT_TEST_DIALOG_H

#include "ui/fragmentTestDialog.ui.h"

class FragmentTestDialog : public QDialog, public Ui::dFragmentTest
{
	Q_OBJECT

	public:
		FragmentTestDialog(QWidget *parent = 0);

		int alphaTestOption() { return m_alphaTest; }
		int depthTestOption() { return m_depthTest; }
		int stencilTestOption() { return m_stencilTest; }
		int blendingOption() { return m_blending; }

		bool copyAlpha() { return m_copyAlpha; }
		bool copyDepth() { return m_copyDepth; }
		bool copyStencil() { return m_copyStencil; }

		float alphaValue() { return m_alphaValue; }
		float depthValue() { return m_depthValue; }
		int   stencilValue() { return m_stencilValue; }
		
	public slots:	
		void setDefaults();
	
	private slots:
		void resetSettings();
		void apply();
		void cancel();
		
	private:
		int m_alphaTest;
		int m_depthTest;
		int m_stencilTest;
		int m_blending;

		bool m_copyAlpha;
		bool m_copyDepth;
		bool m_copyStencil;

		float m_alphaValue;
		float m_depthValue;
		int   m_stencilValue;
};

#endif
