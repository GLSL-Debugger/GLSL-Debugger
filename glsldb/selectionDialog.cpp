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

#include "selectionDialog.qt.h"
#include <QtGui/QTableView>
#include <QtGui/QGridLayout>
#include <QtGui/QImage>
#include <QtGui/QFrame>
#include "colors.qt.h"
#include "vertexTableModel.qt.h"
#include "geoShaderDataModel.qt.h"

#include "dbgprint.h"

/* Fragment Shader */
SelectionDialog::SelectionDialog(PixelBoxFloat *fb, bool elseBranch, QWidget *parent)
    : QDialog(parent)
{
    /* Statistics */
    m_nTotal = 0;
    m_nActive = 0;
    m_nIf = 0;
    m_nElse = 0;
    int x,y;
    
    /* Setup GUI */
    setupUi(this);

    QGridLayout *gridLayout;
    gridLayout = new QGridLayout(fContent);
    gridLayout->setSpacing(0);
    gridLayout->setMargin(0);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setBackgroundRole(QPalette::Dark);
    gridLayout->addWidget(scrollArea);
	
	QImage image;

	if (fb) {
		if (fb->getChannel() != 1) {
			fprintf(stderr, "W! SelectionDialog expects scalar pixelBox\n");
		}
	   
        m_nTotal = fb->getWidth() * fb->getHeight();
		if (m_nTotal == 0) {
			/* Allow all possibilities */
			m_nTotal = 2;
			m_nActive = 2;
			m_nIf = 1;
			m_nElse = 1;
		}
       
		image = QImage(fb->getWidth(), fb->getHeight(), QImage::Format_RGB32);
		for (y=0; y<fb->getHeight(); y++) {
			for (x=0; x<fb->getWidth(); x++) {
				float v;
				if (fb->getDataValue(x, y, &v)) {
					if (v > 0.75) {
						m_nIf++;
						image.setPixel(x, y, DBG_GREEN.rgb());
					} else if (0.25 < v && v < 0.75) {
						m_nElse++;
						image.setPixel(x, y, DBG_RED.rgb());
					}
					m_nActive++;
				} else {
					/* no data, print checkerboard */
					if ( ((x/8)%2) == ((y/8)%2)) {
						image.setPixel(x, y, QColor(255,255,255).rgb());
					} else {
						image.setPixel(x, y, QColor(204,204,204).rgb());
					}
				}
			}
		}
	} else {
        m_nTotal = 2;
		m_nElse = 1;
		m_nActive = 2;
		m_nIf = 1;
		image = QImage(512, 512, QImage::Format_RGB32);
	}
		
    QLabel *label = new QLabel();
    label->setPixmap(QPixmap::fromImage(image));
    scrollArea->setWidget(label);


    /* Activate necessary buttons */
    if (elseBranch) {
        pbElse->setEnabled(true);
    } else {
        pbElse->setEnabled(false);
        lElseCount->setText(QString(""));
    }
    m_bElseBranch = elseBranch;

    /* Show statistic data */
    displayStatistics();

    /* Disable options that no pixel is following */
    if (!m_nIf) {
        pbIf->setEnabled(false);
    }
    if (!m_nElse) {
        pbElse->setEnabled(false);
    }
}

/* Geometry Shader */
SelectionDialog::SelectionDialog(VertexBox *vbCondition, 
                                 QList<ShVarItem*> &watchItems,
								 int inPrimitiveType, int outPrimitiveType,
								 VertexBox *primitiveMap,
                                 VertexBox *vertexCountMap,
                                 bool elseBranch, QWidget *parent)
    : QDialog(parent)
{
    /* Statistics */
    m_nTotal = 0;
    m_nActive = 0;
    m_nIf = 0;
    m_nElse = 0;
    
    /* Setup GUI */
    setupUi(this);

    QGridLayout *gridLayout;
    gridLayout = new QGridLayout(fContent);
    gridLayout->setSpacing(0);
    gridLayout->setMargin(0);

    QTreeView *tv = new QTreeView(this);
	tv->setAlternatingRowColors(true);

    /* Display data and count */
	if (vbCondition) {
        int i;
        float *condition = vbCondition->getDataPointer();
        bool  *coverage  = vbCondition->getCoveragePointer();

        m_nTotal = vbCondition->getNumVertices();
		if (m_nTotal == 0) {
			/* Allow all possibilities */
			m_nTotal = 2;
			m_nActive = 2;
			m_nIf = 1;
			m_nElse = 1;
		}
        
        for (i=0; i<vbCondition->getNumVertices(); i++) {
			if (*coverage) {
				m_nActive++;

	            if (*condition > 0.75f) {
    	                m_nIf++;
        	    } else if (0.25f < *condition && *condition < 0.75f) {
            	        m_nElse++;
                }
			}
			coverage++;
            condition++;
        }

	    GeoShaderDataModel *model = new GeoShaderDataModel(inPrimitiveType,
				outPrimitiveType, primitiveMap, vertexCountMap, vbCondition, NULL);
		for (i=0; i<watchItems.count(); i++) {
			QString name = watchItems[i]->getFullName();
			model->addData(watchItems[i]->getCurrentPointer(), 
					       watchItems[i]->getVertexBoxPointer(), 
						   name);
		}

		tv->setModel(model);
		if (watchItems.count() != 0) {
			tv->setColumnHidden(1, true);
		}
    } else {
    	m_nTotal = 2;
    	m_nActive = 2;
    	m_nIf = 1;
    	m_nElse = 1;
	}

    gridLayout->addWidget(tv);

    /* Activate necessary buttons */
    if (elseBranch) {
        pbElse->setEnabled(true);
    } else {
        pbElse->setEnabled(false);
        lElseCount->setText(QString(""));
    }
    m_bElseBranch = elseBranch;

    /* Show statistic data */
    displayStatistics();

    /* Disable options that no pixel is following */
    if (!m_nIf) {
        pbIf->setEnabled(false);
    }
    if (!m_nElse) {
        pbElse->setEnabled(false);
    }
}

/* Vertex Shader */
SelectionDialog::SelectionDialog(VertexBox *vbCondition, 
                                 QList<ShVarItem*> &watchItems,
                                 bool elseBranch, QWidget *parent)
    : QDialog(parent)
{
    /* Statistics */
    m_nTotal = 0;
    m_nActive = 0;
    m_nIf = 0;
    m_nElse = 0;

    /* Setup GUI */
    setupUi(this);

    QGridLayout *gridLayout;
    gridLayout = new QGridLayout(fContent);
    gridLayout->setSpacing(0);
    gridLayout->setMargin(0);

    QTableView *table = new QTableView(this);
	table->setAlternatingRowColors(true);

    /* Display data and count */
	if (vbCondition) {
        int i;
        float *condition = vbCondition->getDataPointer();
        bool  *coverage  = vbCondition->getCoveragePointer();

        m_nTotal = vbCondition->getNumVertices();
		if (m_nTotal == 0) {
			/* Allow all possibilities */
			m_nTotal = 2;
			m_nActive = 2;
			m_nIf = 1;
			m_nElse = 1;
		}
        
        for (i=0; i<vbCondition->getNumVertices(); i++) {
			if (*coverage) {
				m_nActive++;

	            if (*condition > 0.75f) {
    	                m_nIf++;
        	    } else if (0.25f < *condition && *condition < 0.75f) {
            	        m_nElse++;
                }
			}
			coverage++;
            condition++;
        }

	    VertexTableModel *model = new VertexTableModel();
		for (i=0; i<watchItems.count(); i++) {
			QString name = watchItems[i]->getFullName();
			model->addVertexBox(watchItems[i]->getVertexBoxPointer(), name);
		}
		model->setCondition(vbCondition);


		table->setModel(model);
		if (watchItems.count() != 0) {
			table->setColumnHidden(0, true);
		}


    } else {
		/* Allow all possibilities */
		m_nTotal = 2;
		m_nActive = 2;
		m_nIf = 1;
		m_nElse = 1;
	}

    gridLayout->addWidget(table);
	
    /* Activate necessary buttons */
    if (elseBranch) {
        pbElse->setEnabled(true);
    } else {
        pbElse->setEnabled(false);
        lElseCount->setText(QString(""));
    }
    m_bElseBranch = elseBranch;

    /* Show statistic data */
    displayStatistics();

    /* Disable options that no pixel is following */
    if (!m_nIf) {
        pbIf->setEnabled(false);
    }
    if (!m_nElse) {
        pbElse->setEnabled(false);
    }
}

void SelectionDialog::displayStatistics(void)
{
    QString string;
    
    string = QString(QVariant(m_nActive).toString());
    string.append(" (");
    string.append(QVariant((m_nActive*100)/m_nTotal).toString());
    string.append("%)");
    lActiveCount->setText(string);

    if (m_nActive != 0) {
        string = QString(QVariant(m_nIf).toString());
        string.append(" (");
        string.append(QVariant((m_nIf*100)/(m_nActive)).toString());
        string.append("%)");
        lIfCount->setText(string);
    
        if (m_bElseBranch) {
            string = QString(QVariant(m_nElse).toString());
            string.append(" (");
            string.append(QVariant((m_nElse*100)/(m_nActive)).toString());
            string.append("%)");
            lElseCount->setText(string);
        } else {
            lElseCount->setText("");
        }
    } else {
        lIfCount->setText("");
        lElseCount->setText("");
    }

}

int  SelectionDialog::exec()
{
    /* Skip dialog if there is only one option for user input left */
    if (!m_nActive) {
        return SB_SKIP;
    }

    if (!m_nIf && !m_bElseBranch) {
        return SB_SKIP;
    }

    return QDialog::exec();
}

void SelectionDialog::on_pbSkip_clicked()
{
    done(SB_SKIP);
}

void SelectionDialog::on_pbIf_clicked()
{
    done(SB_IF);
}

void SelectionDialog::on_pbElse_clicked()
{
    done(SB_ELSE);
}

