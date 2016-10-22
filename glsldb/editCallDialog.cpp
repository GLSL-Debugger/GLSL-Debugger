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

#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QLabel>
#include <limits.h>
#include <float.h>

#include "editCallDialog.qt.h"

#ifdef _WIN32
#include <windows.h>
#include "asprintf.h"
#endif /* _WIN32 */

#include "GL/gl.h"
#include "GL/glext.h"
#include "debuglib.h"

extern "C" {
#include "DebugLib/glenumerants.h"
#include "DebugLib/generated/glenumerants.h"
}

class GLEnumValidator: public QValidator {
public:
	GLEnumValidator(QWidget *parant);
	QValidator::State validate(QString &, int &) const;
};

GLEnumValidator::GLEnumValidator(QWidget *parent) :
		QValidator(parent)
{
}

QValidator::State GLEnumValidator::validate(QString &input, int &pos) const
{
	UNUSED_ARG(pos)
	QValidator::State state = QValidator::Invalid;
	int i = 0;

	while (glEnumerantsMap[i].string != NULL) {
		if (input.compare(QString(glEnumerantsMap[i].string)) == 0) {
			/* exact match */
			return QValidator::Acceptable;
		}
		if (QString(glEnumerantsMap[i].string).startsWith(input)) {
			state = QValidator::Intermediate;
		}
		i++;
	}

	return state;
}

EditCallDialog::EditCallDialog(const FunctionCall *fn)
{
	int i;

	setupUi(this);

	m_pChangedCall = new FunctionCall(fn);
	m_qObjects.clear();

	setWindowTitle(QString("Edit ") + QString(fn->getName()));

	QLabel *n = new QLabel(this);
	n->setText(QString(fn->getName() + QString("(")));
	hboxLayout->addWidget(n);

	for (i = 0; i < fn->getNumArguments(); i++) {
		/* Manipulator */
		QWidget *w = getManipulator(fn->getArgument(i));
		if (w) {
			hboxLayout->addWidget(w);
		}
		m_qObjects.append(w);

		/* Labels */
		QLabel *l = new QLabel(this);
		if (i < fn->getNumArguments() - 1) {
			l->setText(", ");
		} else {
			l->setText(")");
		}
		hboxLayout->addWidget(l);
	}
	hboxLayout->addStretch();
}

EditCallDialog::~EditCallDialog()
{
}

void EditCallDialog::checkValidity(void)
{
	QValidator::State state = QValidator::Acceptable;

	int i;
	for (i = 0; i < m_qObjects.size(); i++) {
		const FunctionCall::Argument *arg = m_pChangedCall->getArgument(i);
		switch (arg->iType) {
		case DBG_TYPE_ENUM: {
			QComboBox *cb = (QComboBox*) m_qObjects[i];
			const QValidator *vldr = cb->validator();
			QString s = cb->currentText();
			int pos = 0;
			if (vldr->validate(s, pos) != QValidator::Acceptable) {
				state = QValidator::Invalid;
			}
			break;
		}
		default:
			break;
		}
	}

	/* Find OK button */
	QAbstractButton *okButton = NULL;
	QList<QAbstractButton*> buttonList;
	buttonList = buttonBox->buttons();
	for (i = 0; i < buttonList.size(); i++) {
		if (buttonBox->buttonRole(buttonList[i])
				== QDialogButtonBox::AcceptRole) {
			okButton = buttonList[i];
		}
	}

	if (!okButton) {
		return;
	}

	if (state == QValidator::Acceptable) {
		okButton->setEnabled(true);
	} else {
		okButton->setEnabled(false);
	}
}

const FunctionCall* EditCallDialog::getChangedFunction(void)
{
	int i;

	for (i = 0; i < m_qObjects.size(); i++) {
		const FunctionCall::Argument *arg = m_pChangedCall->getArgument(i);
		switch (arg->iType) {
		case DBG_TYPE_CHAR: {
			char v;
			v = (char) ((QSpinBox*) m_qObjects[i])->value();
			m_pChangedCall->editArgument(i, (void*) &v);
			break;
		}
		case DBG_TYPE_UNSIGNED_CHAR: {
			unsigned char v;
			v = (unsigned char) ((QSpinBox*) m_qObjects[i])->value();
			m_pChangedCall->editArgument(i, (void*) &v);
			break;
		}
		case DBG_TYPE_SHORT_INT: {
			short v;
			v = (short) ((QSpinBox*) m_qObjects[i])->value();
			m_pChangedCall->editArgument(i, (void*) &v);
			break;
		}
		case DBG_TYPE_UNSIGNED_SHORT_INT: {
			unsigned short v;
			v = (unsigned short) ((QSpinBox*) m_qObjects[i])->value();
			m_pChangedCall->editArgument(i, (void*) &v);
			break;
		}
		case DBG_TYPE_INT: {
			int v;
			v = (int) ((QSpinBox*) m_qObjects[i])->value();
			m_pChangedCall->editArgument(i, (void*) &v);
			break;
		}
		case DBG_TYPE_UNSIGNED_INT: {
			unsigned int v;
			v = (unsigned int) ((QSpinBox*) m_qObjects[i])->value();
			m_pChangedCall->editArgument(i, (void*) &v);
			break;
		}
		case DBG_TYPE_LONG_INT: {
			long v;
			v = (long) ((QSpinBox*) m_qObjects[i])->value();
			m_pChangedCall->editArgument(i, (void*) &v);
			break;
		}
		case DBG_TYPE_UNSIGNED_LONG_INT: {
			unsigned long v;
			v = (unsigned long) ((QSpinBox*) m_qObjects[i])->value();
			m_pChangedCall->editArgument(i, (void*) &v);
			break;
		}
		case DBG_TYPE_LONG_LONG_INT: {
			long long v;
			v = (long long) ((QSpinBox*) m_qObjects[i])->value();
			m_pChangedCall->editArgument(i, (void*) &v);
			break;
		}
		case DBG_TYPE_UNSIGNED_LONG_LONG_INT: {
			unsigned long long v;
			v = (unsigned long long) ((QSpinBox*) m_qObjects[i])->value();
			m_pChangedCall->editArgument(i, (void*) &v);
			break;
		}
		case DBG_TYPE_FLOAT: {
			float v;
			v = (float) ((QDoubleSpinBox*) m_qObjects[i])->value();
			m_pChangedCall->editArgument(i, (void*) &v);
			break;
		}
		case DBG_TYPE_DOUBLE: {
			double v;
			v = (double) ((QDoubleSpinBox*) m_qObjects[i])->value();
			m_pChangedCall->editArgument(i, (void*) &v);
			break;
		}
		case DBG_TYPE_POINTER:
			/* So far do not support change of pointers */
			break;
		case DBG_TYPE_BOOLEAN: {
			bool v;
			v = (bool) ((QComboBox*) m_qObjects[i])->currentIndex();
			m_pChangedCall->editArgument(i, (void*) &v);
			break;
		}
		case DBG_TYPE_BITFIELD: {
			GLbitfield v = 0;
			int eIdx = 0;
			QListWidget* list = (QListWidget*) m_qObjects[i];

			for (eIdx = 0; eIdx < list->count(); eIdx++) {
				if (list->item(eIdx)->isSelected()) {
					v |= list->item(eIdx)->data(Qt::UserRole).toInt();
				}
			}
			m_pChangedCall->editArgument(i, (void*) &v);
			break;
		}
		case DBG_TYPE_ENUM: {
			GLenum v;
			int eIdx = 0;
			QString s = ((QComboBox*) m_qObjects[i])->currentText();

			while (glEnumerantsMap[i].string != NULL) {
				if (!(s.compare(QString(glEnumerantsMap[eIdx].string)))) {
					v = (GLenum) glEnumerantsMap[eIdx].value;
					m_pChangedCall->editArgument(i, (void*) &v);
					break;
				}
				eIdx++;
			}
			break;
		}
		case DBG_TYPE_STRUCT:
			/* don't know how to change a struct */
			break;
		default:
			break;
		}
	}

	return m_pChangedCall;
}

QWidget* EditCallDialog::getManipulator(const FunctionCall::Argument *arg)
{
	QWidget *w;
	switch (arg->iType) {
	case DBG_TYPE_CHAR:
		w = new QSpinBox(this);
		((QSpinBox*) w)->setRange(CHAR_MIN, CHAR_MAX);
		((QSpinBox*) w)->setValue(*(char*) arg->pData);
		break;
	case DBG_TYPE_UNSIGNED_CHAR:
		w = new QSpinBox(this);
		((QSpinBox*) w)->setRange(0, UCHAR_MAX);
		((QSpinBox*) w)->setValue(*(unsigned char*) arg->pData);
		break;
	case DBG_TYPE_SHORT_INT:
		w = new QSpinBox(this);
		((QSpinBox*) w)->setRange(SHRT_MIN, SHRT_MAX);
		((QSpinBox*) w)->setValue(*(short*) arg->pData);
		break;
	case DBG_TYPE_UNSIGNED_SHORT_INT:
		w = new QSpinBox(this);
		((QSpinBox*) w)->setRange(0, USHRT_MAX);
		((QSpinBox*) w)->setValue(*(unsigned short*) arg->pData);
		break;
	case DBG_TYPE_INT:
		w = new QSpinBox(this);
		((QSpinBox*) w)->setRange(INT_MIN, INT_MAX);
		((QSpinBox*) w)->setValue(*(int*) arg->pData);
		break;
	case DBG_TYPE_UNSIGNED_INT:
		w = new QSpinBox(this);
		//((QSpinBox*)w)->setRange(0, UINT_MAX);
		((QSpinBox*) w)->setRange(0, INT_MAX);
		((QSpinBox*) w)->setValue(*(unsigned int*) arg->pData);
		break;
	case DBG_TYPE_LONG_INT:
		w = new QSpinBox(this);
		//((QSpinBox*)w)->setRange(LONG_MIN, LONG_MAX);
		((QSpinBox*) w)->setRange(INT_MIN, INT_MAX);
		((QSpinBox*) w)->setValue(*(long*) arg->pData);
		break;
	case DBG_TYPE_UNSIGNED_LONG_INT:
		w = new QSpinBox(this);
		//((QSpinBox*)w)->setRange(0, ULONG_MAX);
		((QSpinBox*) w)->setRange(0, INT_MAX);
		((QSpinBox*) w)->setValue(*(unsigned long*) arg->pData);
		break;
	case DBG_TYPE_LONG_LONG_INT:
		w = new QSpinBox(this);
		//((QSpinBox*)w)->setRange(LLONG_MIN, LLONG_MAX);
		((QSpinBox*) w)->setRange(INT_MIN, INT_MAX);
		((QSpinBox*) w)->setValue(*(long long*) arg->pData);
		break;
	case DBG_TYPE_UNSIGNED_LONG_LONG_INT:
		w = new QSpinBox(this);
		//((QSpinBox*)w)->setRange(0, ULLONG_MAX);
		((QSpinBox*) w)->setRange(0, INT_MAX);
		((QSpinBox*) w)->setValue(*(unsigned long long*) arg->pData);
		break;
	case DBG_TYPE_FLOAT:
		w = new QDoubleSpinBox(this);
		((QDoubleSpinBox*) w)->setRange(-FLT_MAX, FLT_MAX);
		((QDoubleSpinBox*) w)->setDecimals(6);
		((QDoubleSpinBox*) w)->setValue(*(float*) arg->pData);
		break;
	case DBG_TYPE_DOUBLE:
		w = new QDoubleSpinBox(this);
		((QDoubleSpinBox*) w)->setRange(-DBL_MAX, DBL_MAX);
		((QDoubleSpinBox*) w)->setDecimals(6);
		((QDoubleSpinBox*) w)->setValue(*(float*) arg->pData);
		break;
	case DBG_TYPE_POINTER: {
		char *s;
		w = new QLineEdit(this);
		((QLineEdit*) w)->setEnabled(false);
		asprintf(&s, "%p", *(void**) arg->pData);
		((QLineEdit*) w)->setText(s);
		break;
	}
	case DBG_TYPE_BOOLEAN:
		w = new QComboBox(this);
		((QComboBox*) w)->addItem("false");
		((QComboBox*) w)->addItem("true");
		((QComboBox*) w)->setCurrentIndex(*(bool*) arg->pData);
		break;
	case DBG_TYPE_BITFIELD: {
		int i = 0;
		GLbitfield b = *(GLbitfield*) arg->pData;
		w = new QListWidget(this);
		((QListWidget*) w)->setSortingEnabled(true);
		((QListWidget*) w)->setSelectionMode(QAbstractItemView::MultiSelection);
		while (glBitfieldMap[i].string != NULL) {
			QListWidgetItem *item = new QListWidgetItem((QListWidget*) w);
			item->setText(QString(glBitfieldMap[i].string));
			item->setData(Qt::UserRole, QVariant(glBitfieldMap[i].value));
			if ((glBitfieldMap[i].value & b) == glBitfieldMap[i].value) {
				item->setSelected(true);
			}
			i++;
		}
		((QListWidget*) w)->setMinimumSize(230, 150);
		break;
	}
	case DBG_TYPE_ENUM: {
		int i = 0;
		int initialized = 0;
		w = new QComboBox(this);
		GLEnumValidator *v = new GLEnumValidator(w);
		((QComboBox*) w)->setEditable(true);
		((QComboBox*) w)->setInsertPolicy(QComboBox::NoInsert);
		((QComboBox*) w)->setMaxVisibleItems(15);
		((QComboBox*) w)->setValidator(v);
		while (glEnumerantsMap[i].string != NULL) {
			((QComboBox*) w)->addItem(glEnumerantsMap[i].string);
			if (*(GLenum*) arg->pData == glEnumerantsMap[i].value
					&& !initialized) {
				((QComboBox*) w)->setCurrentIndex(i);
				initialized = 1;
			}
			i++;
		}
		connect(w, SIGNAL(editTextChanged(QString)), this,
				SLOT(checkValidity()));
		break;
	}
	case DBG_TYPE_STRUCT: {
		w = new QLineEdit(this);
		((QLineEdit*) w)->setEnabled(false);
		((QLineEdit*) w)->setText("STRUCT");
		break;
	}
	default:
		return NULL;
	}
	return w;
}

