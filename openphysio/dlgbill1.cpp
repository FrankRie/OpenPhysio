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

#include "dlgbill1.h"
#include "browse.h"

DlgBill1::DlgBill1(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
	
    ui.deEnd->setDate(QDate::currentDate());
    ui.deStart->setDate(QDate::currentDate().addDays(-60));

    browseReceipes = new BrowseReceipes(BrowseReceipes::MODE_BILL);
    setupFilter();
    ui.gridLayout->addWidget(browseReceipes, 2, 0, 1, 9);
	
    connect(ui.pbCancel, &QPushButton::clicked, this, &DlgBill1::slotCancel);
    connect(ui.pbNext, &QPushButton::clicked, this, &DlgBill1::slotNext);
    connect(ui.deStart, &QDateEdit::dateChanged, this, &DlgBill1::slotUpdate);
    connect(ui.deEnd, &QDateEdit::dateChanged, this, &DlgBill1::slotUpdate);
}


DlgBill1::~DlgBill1()
{
    if (browseReceipes != NULL) delete browseReceipes;
}


void DlgBill1::setupFilter()
{
    QString s;
    QDate d;
	
    //tf.clear();
	
    // TODO: remove hard-coded value for "laufend" and "gesetzlich"
    //tf.addFilterExpr(TableFilter::F_REC_STATE, TableFilter::OP_EQUAL, 0);
    //tf.addFilterExpr(TableFilter::F_REC_TYPE, TableFilter::OP_EQUAL, 0);
	
    d = ui.deStart->date();
    s = d.toString("yyyyMMdd");
    //tf.addFilterExpr(TableFilter::F_REC_RDATE, TableFilter::OP_GREATER_OR_EQUAL, s);
    d = ui.deEnd->date();
    s = d.toString("yyyyMMdd");
    //tf.addFilterExpr(TableFilter::F_REC_RDATE, TableFilter::OP_LOWER_OR_EQUAL, s);
	
    //browseReceipes->setFilter(&tf);
}


void DlgBill1::slotCancel()
{
    this->reject();
}


void DlgBill1::slotNext()
{
    this->accept();
}


void DlgBill1::slotUpdate()
{
    setupFilter();
}
