/****************************************************************************
**
** Copyright (C) 2013 Sebastian Vandersee
**               2026 Dr. Frankl Riechert 
** All rights reserved.
**
** This file is part of OpenPhysio.
**
** Please refer to the LICENSE file for terms of use.
**
****************************************************************************/

#include "browsedocs.h"
#include "ui_browsedocs.h"
#include "gendoc.h"
#include "browse.h"


BrowseDocs::BrowseDocs(QWidget *parent, int mask, int fid) :
    QWidget(parent),
    m_ui(new Ui::BrowseDocs)
{
    m_ui->setupUi(this);

    curView = -1;
    browseInvoices = NULL;
    browsePraxisInvoices = NULL;
    browseQuittances = NULL;
    browseReminders = NULL;
    browseReports = NULL;
    browseFindings = NULL;
    browseTemplates = NULL;

    last_inserted_widget = NULL;

    listMask.clear();
    if (mask & MASK_INVOICES) {
	qslNames << "Rechnungen";
        listMask << MASK_INVOICES;
        browseInvoices = new BrowseInvoices();
    }
    if (mask & MASK_PINVOICES) {
        qslNames << "Praxisrechnungen";
        listMask <<  MASK_PINVOICES;
        browsePraxisInvoices = new BrowsePraxisInvoices();
    }
    if (mask & MASK_REMINDERS) {
	qslNames << "Mahnungen";
        listMask << MASK_REMINDERS;
        browseReminders = new BrowseReminders();
    }
    if (mask & MASK_QUITTANCES) {
	qslNames << "Quittungen";
        listMask << MASK_QUITTANCES;
        browseQuittances = new BrowseQuittances();
    }
    if (mask & MASK_REPORTS) {
	qslNames << "Therapieberichte";
        listMask << MASK_REPORTS;
        browseReports = new BrowseReports();
    }
    if (mask & MASK_FINDINGS) {
	qslNames << "Befunde";
        listMask << MASK_FINDINGS;
        browseFindings = new BrowseFindings();
    }
    if (mask & MASK_TEMPLATES) {
        qslNames << "Vorlagen";
        listMask << MASK_TEMPLATES;
        browseTemplates = new BrowseDocTemplates();
    }

    //tf.clear();
    filter_id = fid;
    mode = MODE_FULL;

    for (int i = 0; i < qslNames.count(); i++) {
	items.append(new QTreeWidgetItem(m_ui->treeWidget));
	items.at(i)->setText(0, qslNames.at(i));
	items.at(i)->setIcon(0, QIcon(":/report.png"));
    }

    //m_ui->treeWidget->setCurrentItem(items.at(0), 0);

    connect(m_ui->treeWidget, &QTreeWidget::currentItemChanged,this, &BrowseDocs::slotDocSelected);
}



BrowseDocs::~BrowseDocs()
{
    disconnect(m_ui->treeWidget, &QTreeWidget::currentItemChanged, this, &BrowseDocs::slotDocSelected);
    while (!items.isEmpty()) delete items.takeFirst();
    QGridLayout *qgl = (QGridLayout *) m_ui->gridLayout;
    if ( (last_inserted_widget != NULL) && (qgl != NULL) ) qgl->removeWidget(last_inserted_widget);

    if (browseInvoices != NULL) delete browseInvoices;
    //if (browsePraxisInvoices != NULL) delete browsePraxisInvoices;
    if (browseReminders != NULL) delete browseReminders;
    if (browseQuittances != NULL) delete browseQuittances;
    if (browseReports != NULL) delete browseReports;
    if (browseFindings != NULL) delete browseFindings;
    if (browseTemplates != NULL) delete browseTemplates;

    delete m_ui;
}


void BrowseDocs::setMode(int m)
{
    mode = m;
}


void BrowseDocs::setFilterID(int id)
{
    filter_id = id;
}


void BrowseDocs::removeCurrentView()
{
    if ( (curView < 0) || (curView >= listMask.count()) ) return;
    int actual_view = listMask.at(curView);
    switch(actual_view) {
        case MASK_INVOICES:
            //if (browseInvoices != NULL) delete browseInvoices;
            //browseInvoices = NULL;
            browseInvoices->hide();
            break;
        case MASK_PINVOICES:
            //if (browseInvoices != NULL) delete browseInvoices;
            //browseInvoices = NULL;
            browsePraxisInvoices->hide();
            break;
        case MASK_REMINDERS:
            //if (browseReminders != NULL) delete browseReminders;
            //browseReminders = NULL;
            browseReminders->hide();
	    break;
        case MASK_QUITTANCES:
            //if (browseQuittances != NULL) delete browseQuittances;
            //browseQuittances = NULL;
            browseQuittances->hide();
	    break;
        case MASK_REPORTS:
            //if (browseReports != NULL) delete browseReports;
            //browseReports = NULL;
            browseReports->hide();
	    break;
        case MASK_FINDINGS:
            //if (browseFindings != NULL) delete browseFindings;
            //browseFindings = NULL;
            browseFindings->hide();
	    break;
        case MASK_TEMPLATES:
            //if (browseTemplates != NULL) delete browseTemplates;
            //browseTemplates = NULL;
            browseTemplates->hide();
            break;
        default:
	    break;
    }
}


void BrowseDocs::Reload()
{
    if (curView == -1) {
	loadDocs(0);
    } else {
        removeCurrentView();
	int x = curView;
	curView = -1;
	loadDocs(x);
    }
}

/*
void BrowseDocs::loadDocs(int d)
{
    //qDebug() << __FUNCTION__ << "->";
    int filter_field = -1;

    if (d == curView) return;
    removeCurrentView();
    QWidget *widget = NULL;
    if ( (d < 0) || (d >= listMask.count()) ) return;
    int actual_view = listMask.at(d);
    switch(actual_view) {
	case GenDoc::DOC_INVOICE:
	    if (browseInvoices == NULL) {
		browseInvoices = new BrowseInvoices();
		if (filter_id >= 0) {
                    if (mode == MODE_RECEIPE) filter_field = Browse::F_INVOICE_RID;
                    if (mode == MODE_PATIENT) filter_field = Browse::F_INVOICE_PID;
                    //if (mode == MODE_RECEIPE) tf.addFilterExpr(TableFilter::F_INVOICE_RID, TableFilter::OP_EQUAL, filter_id);
                    //if (mode == MODE_PATIENT) tf.addFilterExpr(TableFilter::F_INVOICE_PID, TableFilter::OP_EQUAL, filter_id);
                }
	    }
	    widget = (QWidget*)browseInvoices;
	    break;
	case GenDoc::DOC_REMINDER:
	    if (browseReminders == NULL) {
		browseReminders = new BrowseReminders();
		if (filter_id >= 0) {
                    if (mode == MODE_RECEIPE) filter_field = Browse::F_REMINDER_RID;
                    if (mode == MODE_PATIENT) filter_field = Browse::F_REMINDER_PID;
                    //if (mode == MODE_RECEIPE) tf.addFilterExpr(TableFilter::F_REMINDER_RID, TableFilter::OP_EQUAL, filter_id);
                    //if (mode == MODE_PATIENT) tf.addFilterExpr(TableFilter::F_REMINDER_PID, TableFilter::OP_EQUAL, filter_id);
                }
	    }
	    widget = (QWidget*)browseReminders;
	    break;
	case GenDoc::DOC_QUITTANCE:
	    if (browseQuittances == NULL) {
		browseQuittances = new BrowseQuittances();
		if (filter_id >= 0) {
		    if (mode == MODE_RECEIPE) filter_field = Browse::F_QUITTANCE_RID;
		    if (mode == MODE_PATIENT) filter_field = Browse::F_QUITTANCE_PID;
		    //if (mode == MODE_RECEIPE) tf.addFilterExpr(TableFilter::F_QUITTANCE_RID, TableFilter::OP_EQUAL, filter_id);
		    //if (mode == MODE_PATIENT) tf.addFilterExpr(TableFilter::F_QUITTANCE_PID, TableFilter::OP_EQUAL, filter_id);
		}
	    }
	    widget = (QWidget*)browseQuittances;
	    break;
	case GenDoc::DOC_REPORT:
	    if (browseReports == NULL) {
		browseReports = new BrowseReports();
		if (filter_id >= 0) {
		    if (mode == MODE_RECEIPE) filter_field = Browse::F_REPORT_RID;
		    if (mode == MODE_PATIENT) filter_field = Browse::F_REPORT_PID;
		    //if (mode == MODE_RECEIPE) tf.addFilterExpr(TableFilter::F_REPORT_RID, TableFilter::OP_EQUAL, filter_id);
		    //if (mode == MODE_PATIENT) tf.addFilterExpr(TableFilter::F_REPORT_PID, TableFilter::OP_EQUAL, filter_id);
		}
	    }
	    widget = (QWidget*)browseReports;
	    break;
	case GenDoc::DOC_FINDING:
	    if (browseFindings == NULL) {
                if (mode == MODE_PATIENT) browseFindings = new BrowseFindings(BrowseFindings::MODE_FULL);
		else browseFindings = new BrowseFindings();
		if ((filter_id >= 0) && (mode == MODE_PATIENT)) {
		    filter_field = Browse::F_FINDING_PID;
		    //tf.addFilterExpr(TableFilter::F_FINDING_PID, TableFilter::OP_EQUAL, filter_id);
		    // inform the browser which patient is currently active
		    browseFindings->setPatientID(filter_id);
		}
	    }
	    widget = (QWidget*)browseFindings;
	    break;
        case GenDoc::DOC_TEMPLATES:
            if (browseTemplates == NULL) {
                browseTemplates = new BrowseDocTemplates();
            }
            widget = (QWidget*)browseTemplates;
            break;
        default:
	    break;
    }

    if (last_inserted_widget != NULL) {
	QGridLayout *qgl = (QGridLayout *) m_ui->gridLayout;
	qgl->removeWidget(last_inserted_widget);
    }

    if (widget != NULL) {
        // BrowseDocTemplates does not inherit from class Browse ...
        if ( (filter_id > 0) && (filter_field > 0) ) {
            TableFilter tf(((Browse*)widget)->getTableType());
            tf.addFilterExpr(false, filter_field, TableFilter::OP_EQUAL, filter_id);
            ((Browse*)widget)->setFilter(&tf);
        }
        // BrowseDocTemplates does not inherit from class Browse ...
        //if (actual_view != GenDoc::DOC_TEMPLATES) {
        //    ((Browse*)widget)->setFilter(filter_field, filter_id, Browse::OP_EQUAL);
        //}
	QGridLayout *qgl = (QGridLayout *) m_ui->gridLayout;
	qgl->addWidget(widget, 0, 2, 1, 12);
	last_inserted_widget = widget;
    }
    curView = d;
    //qDebug() << __FUNCTION__ << "<-";
}
*/

void BrowseDocs::loadDocs(int d)
{
    int filter_field = -1;

    if (d == curView) return;
    removeCurrentView();
    QWidget *widget = NULL;
    if ( (d < 0) || (d >= listMask.count()) ) return;
    int actual_view = listMask.at(d);
    switch(actual_view) {
        case MASK_INVOICES:
            if (filter_id >= 0) {
                if (mode == MODE_RECEIPE) filter_field = Browse::F_INVOICE_RID;
                else if (mode == MODE_PATIENT) filter_field = Browse::F_INVOICE_PID;
            }
            widget = (QWidget*)browseInvoices;
            break;
        case MASK_PINVOICES:
           /* if (filter_id >= 0) {
                if (mode == MODE_RECEIPE) filter_field = Browse::F_INVOICE_RID;
                else if (mode == MODE_PATIENT) filter_field = Browse::F_INVOICE_PID;
            } */
            widget = (QWidget*)browsePraxisInvoices;
            break;
        case MASK_REMINDERS:
            if (filter_id >= 0) {
                if (mode == MODE_RECEIPE) filter_field = Browse::F_REMINDER_RID;
                else if (mode == MODE_PATIENT) filter_field = Browse::F_REMINDER_PID;
            }
            widget = (QWidget*)browseReminders;
            break;
        case MASK_QUITTANCES:
            if (filter_id >= 0) {
                if (mode == MODE_RECEIPE) filter_field = Browse::F_QUITTANCE_RID;
                else if (mode == MODE_PATIENT) filter_field = Browse::F_QUITTANCE_PID;
            }
            widget = (QWidget*)browseQuittances;
            break;
        case MASK_REPORTS:
            if (filter_id >= 0) {
                if (mode == MODE_RECEIPE) filter_field = Browse::F_REPORT_RID;
                else if (mode == MODE_PATIENT) filter_field = Browse::F_REPORT_PID;
            }
            widget = (QWidget*)browseReports;
            break;
        case MASK_FINDINGS:
            if ((filter_id >= 0) && (mode == MODE_PATIENT)) {
                filter_field = Browse::F_FINDING_PID;
                // inform the browser which patient is currently active
                browseFindings->setPatientID(filter_id);
            }
            widget = (QWidget*)browseFindings;
            break;
        case MASK_TEMPLATES:
            widget = (QWidget*)browseTemplates;
            break;
        default:
            break;
    }

    if (last_inserted_widget != NULL) {
        QGridLayout *qgl = (QGridLayout *) m_ui->gridLayout;
        qgl->removeWidget(last_inserted_widget);
    }

    if (widget != NULL) {
        // BrowseDocTemplates does not inherit from class Browse ...
        if ( (filter_id > 0) && (filter_field > 0) ) {
            TableFilter tf(((Browse*)widget)->getTableType());
            tf.addFilterExpr(true, filter_field, TableFilter::OP_EQUAL, filter_id);
            ((Browse*)widget)->setFilter(&tf);
        }
        // BrowseDocTemplates does not inherit from class Browse ...
        /*if (actual_view != GenDoc::DOC_TEMPLATES) {
            ((Browse*)widget)->setFilter(filter_field, filter_id, Browse::OP_EQUAL);
        }*/
        QGridLayout *qgl = (QGridLayout *) m_ui->gridLayout;
        qgl->addWidget(widget, 0, 2, 1, 12);
        last_inserted_widget = widget;
        widget->show();
    }
    curView = d;
}


void BrowseDocs::slotDocSelected(QTreeWidgetItem *w)
{
    if (w == NULL) return;
    if (w->columnCount() < 1) return;

    for (int i = 0; i < qslNames.count(); i++) {
	if (w->text(0) == qslNames.at(i)) {
	    loadDocs(i);
	    break;
	}
    }
}


void BrowseDocs::Preload()
{
    if (myConf.usePreload()) {
        if (browseInvoices != NULL) browseInvoices->preload();
        if (browsePraxisInvoices != NULL) browsePraxisInvoices->preload();
        if (browseQuittances != NULL) browseQuittances->preload();
        if (browseReminders != NULL) browseReminders->preload();
        if (browseReports != NULL) browseReports->preload();
        if (browseFindings != NULL) browseFindings->preload();
    }
}
