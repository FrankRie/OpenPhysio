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

#include "browsefindings.h"
#include "ui_browsefindings.h"
#include "mydb.h"
#include "msgbox.h"
#include "dlgdocfinding.h"
#include "protect.h"


BrowseFindings::BrowseFindings(int mode, QWidget *parent) :
    Browse(mode, parent),
    m_ui(new Ui::BrowseFindings)
{
    m_ui->setupUi(this);
    init((TableViewAnimated*)m_ui->tvFindings, myDB::TABLE_FINDINGS, 1, Qt::DescendingOrder);

    // add display filter widget and toolbar
    QGridLayout *layout = (QGridLayout*) this->layout();
    layout->addWidget(&browseToolBar, 0, 0);
    layout->addWidget(&displayFilter, 0, 1);

    pid = -1;

    actFindingAdd = NULL;
    actFindingView = NULL;
    actFindingDel = NULL;

    if (mode == MODE_SELECT) {
        m_ui->widget->hide();
    }
    actFindingAdd = new QAction(QString::fromUtf8("Befund hinzufügen"), this);
    connect(actFindingAdd, &QAction::triggered, this, &BrowseFindings::slotFindingAdd);
    connect(m_ui->pbAdd, &QPushButton::clicked, this, &BrowseFindings::slotFindingAdd);
    connect(m_ui->pbDel, &QPushButton::clicked, this, &BrowseFindings::slotFindingDel);
    actFindingView = new QAction(QIcon(":/edit.png"), QString::fromUtf8("Befund ansehen/bearbeiten"), this);
    actFindingDel = new QAction(QIcon(":/delete.png"), QString::fromUtf8("Befund löschen"), this);
    connect(actFindingView, &QAction::triggered, this, &BrowseFindings::slotFindingSelected);
    connect(actFindingDel, &QAction::triggered, this, &BrowseFindings::slotFindingDel);
    browseToolBar.addAction(actFindingView);
    browseToolBar.addSeparator();
    browseToolBar.addAction(actFindingDel);
    m_ui->tvFindings->addAction(actFindingView);
    m_ui->tvFindings->addAction(actFindingDel);
    if (mode == MODE_FULL) m_ui->tvFindings->addAction(actFindingAdd);
    m_ui->tvFindings->setContextMenuPolicy(Qt::ActionsContextMenu);

    connect(m_ui->tvFindings, &QTableView::activated, this, &BrowseFindings::slotFindingSelected);
}


BrowseFindings::~BrowseFindings()
{
    if (actFindingAdd != NULL) delete actFindingAdd;
    if (actFindingView != NULL) delete actFindingView;
    if (actFindingDel != NULL) delete actFindingDel;
    delete m_ui;
}


void BrowseFindings::setPatientID(int id)
{
    // we need to know which patient is currently active
    pid = id;
    if (id > 0) m_ui->widget->show();
}


void BrowseFindings::slotFindingSelected()
{
    GUARD(Protect::FUNC_FIND);

    int id = getSelectedItem();
    if (id < 0) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Bitte erst einen Befund wählen!\n"), QMessageBox::Ok);
	return;
    }

    cFinding f;
    if (db.retrieveItem(f, id)) {
	DlgDocFinding dlg;
	dlg.setWindowTitle("Befund bearbeiten");
	dlg.init(f);
	if (dlg.exec() == QDialog::Accepted) {
	}
    }
}


void BrowseFindings::slotFindingDel()
{
    GUARD(Protect::FUNC_FIND);

    int id = getSelectedItem();
    if (id < 0) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Bitte erst einen Befund wählen!\n"), QMessageBox::Ok);
	return;
    }

    MsgBox box;
    if (box.execYesNoBox("Befund wirklich loeschen?\n", MsgBox::BTN_NO) == MsgBox::BTN_YES) {
	if (db.deleteItem(myDB::TABLE_FINDINGS, id) == false) {
            QMessageBox::critical(0, QObject::tr("Datenbankfehler"), QString::fromUtf8("Befund konnte nicht gelöscht werden."));
	}
    }
}


void BrowseFindings::slotFindingAdd()
{
    GUARD(Protect::FUNC_FIND);

    if (db.listEmpl.count() < 1) {
        QMessageBox::information(this, "OpenPhysio", QString::fromUtf8("Es muss mindestens ein Mitarbeiter bereits angelegt sein!"), QMessageBox::Ok);
        return;
    }

    if (pid > 0) {  // only show the dialog if patient is valid (already saved)
	cFinding f;
	f.setPatientID(pid);
	DlgDocFinding dlg;
	dlg.init(f);
	dlg.exec();
    } else {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Die Patientendaten müssen erst gespeichert werden!\n"), QMessageBox::Ok);
    }
}
