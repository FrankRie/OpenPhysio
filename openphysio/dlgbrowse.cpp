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

#include "dlgbrowse.h"
#include "ui_dlgbrowse.h"


DlgBrowse::DlgBrowse(Browse *b, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgBrowse)
{
    ui->setupUi(this);
    browse = b;

    gridLayout = new QGridLayout(this);
    gridLayout->addWidget(browse, 0, 0, 1, 1);

    generateDlgTitle();

    connect(browse, SIGNAL(sigItemSelected()), this, SLOT(slotSelected()));
}


DlgBrowse::~DlgBrowse()
{
    delete gridLayout;
    delete ui;
}


void DlgBrowse::slotSelected()
{
    this->accept();
}


void DlgBrowse::generateDlgTitle()
{
    if (browse == NULL) return;

    QString s = "";
    int m = browse->getMode();
    int t = browse->getTableType();

    switch(t) {
    case myDB::TABLE_COURSES: s = "Kurs"; break;
    case myDB::TABLE_PATIENTS: s = (m == Browse::MODE_MULTISELECT) ? "Patienten" : "Patient"; break;
    case myDB::TABLE_RECEIPES: s = "Rezept"; break;
    case myDB::TABLE_INSURANCES: s = QString::fromUtf8("Kostenträger"); break;
    case myDB::TABLE_PHYSICIANS: s = "Arzt"; break;
    case myDB::TABLE_ROUTES: s = "Route"; break;
    default: break;
    }

    if (m == Browse::MODE_MULTISELECT) {
        s += QString::fromUtf8(" auswählen");
    } else {
        s += QString::fromUtf8(" auswählen (Doppelklick oder Enter)");
    }

    setWindowTitle(s);
}
