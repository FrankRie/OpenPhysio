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

#include "browsereports.h"
#include "ui_browsereports.h"
#include "gendoc.h"
#include "mydb.h"
#include "msgbox.h"
#include "protect.h"


BrowseReports::BrowseReports(int mode, QWidget *parent) :
    Browse(mode, parent),
    m_ui(new Ui::BrowseReports)
{
    m_ui->setupUi(this);
    init((TableViewAnimated*)m_ui->tvReports, myDB::TABLE_REPORTS, 1, Qt::DescendingOrder);

    // add display filter widget and toolbar
    QGridLayout *layout = (QGridLayout*) this->layout();
    layout->addWidget(&browseToolBar, 0, 0);
    layout->addWidget(&displayFilter, 0, 1);

    actReportView = new QAction(QIcon(":/edit.png"), QString::fromUtf8("Bericht ansehen/bearbeiten"), this);
    actReportDel = new QAction(QIcon(":/delete.png"), QString::fromUtf8("Bericht löschen"), this);
    connect(actReportView, SIGNAL(triggered()), this, SLOT(slotReportSelected()));
    connect(actReportDel, SIGNAL(triggered()), this, SLOT(slotReportDel()));
    browseToolBar.addAction(actReportView);
    browseToolBar.addSeparator();
    browseToolBar.addAction(actReportDel);
    m_ui->tvReports->addAction(actReportView);
    m_ui->tvReports->addAction(actReportDel);
    m_ui->tvReports->setContextMenuPolicy(Qt::ActionsContextMenu);

    connect(m_ui->tvReports, SIGNAL(activated(const QModelIndex&)), this, SLOT(slotReportSelected()));
}


BrowseReports::~BrowseReports()
{
    if (actReportView != NULL) delete actReportView;
    if (actReportDel != NULL) delete actReportDel;
    delete m_ui;
}


void BrowseReports::slotReportSelected()
{
    GUARD(Protect::FUNC_REP);

    int id = getSelectedItem();
    if (id < 0) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Bitte erst einen Bericht wählen!\n"), QMessageBox::Ok);
	return;
    }

    cReport rep;
    if (db.retrieveItem(rep, id)) gendoc.genReport(rep);
}


void BrowseReports::slotReportDel()
{
    GUARD(Protect::FUNC_REP);

    int id = getSelectedItem();
    if (id < 0) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Bitte erst einen Bericht wählen!\n"), QMessageBox::Ok);
	return;
    }

    MsgBox box;
    if (box.execYesNoBox(QString::fromUtf8("Bericht wirklich löschen?\n"), MsgBox::BTN_NO) == MsgBox::BTN_YES) {
	if (db.deleteItem(myDB::TABLE_REPORTS, id) == false) {
            QMessageBox::critical(0, QObject::tr("Datenbankfehler"), QString::fromUtf8("Bericht konnte nicht gelöscht werden."));
	}
    }
}
