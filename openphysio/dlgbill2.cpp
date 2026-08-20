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

#include "dlgbill2.h"
#include "browse.h"
#include "mydb.h"


DlgBill2::DlgBill2(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    res = RES_CANCEL;
	
    ui.deEnd->setDate(QDate::currentDate());
    ui.deStart->setDate(QDate::currentDate().addDays(-60));

    browseReceipes = new BrowseReceipes(BrowseReceipes::MODE_BILL);
//    browseReceipes->enableBackgroundColorMode(true, "Abgerechnet", 6, 90, 190, 90);

    //browseReceipes->RegisterUpdateSignal(this);
    //connect(&db, SIGNAL(sigDBChange(int)), this, SLOT(slotUpdate(int)));
    slotSetupFilter();
	
    ui.gridLayout->addWidget(browseReceipes, 2, 0, 12, 6);
	
    connect(ui.pbCancel, &QPushButton::clicked, this, &DlgBill2::slotCancel);
    connect(ui.pbBack, &QPushButton::clicked, this, &DlgBill2::slotBack);
    connect(ui.pbSave, &QPushButton::clicked, this, &DlgBill2::slotSave);
    connect(ui.pbRecSelect, &QPushButton::clicked, this, &DlgBill2::slotBillSelected);
    connect(ui.pbRecAll, &QPushButton::clicked, this, &DlgBill2::slotBillAll);
	
    connect(ui.deStart,  &QDateEdit::userDateChanged, this, &DlgBill2::slotSetupFilter);
    connect(ui.deEnd,  &QDateEdit::userDateChanged, this, &DlgBill2::slotSetupFilter);
}


DlgBill2::~DlgBill2()
{
    if (browseReceipes != NULL) delete browseReceipes;
}


void DlgBill2::slotSetupFilter()
{
    QString s;
    QDate d;
	
    //tf.clear();
	
    // TODO: remove hard-coded values
    //tf.addFilterExpr(TableFilter::F_REC_STATE, TableFilter::OP_GREATER, 0);
    //tf.addFilterExpr(TableFilter::F_REC_TYPE, TableFilter::OP_EQUAL, 0);
	
    d = ui.deStart->date();
    s = d.toString("yyyyMMdd");
    //tf.addFilterExpr(TableFilter::F_REC_RDATE, TableFilter::OP_GREATER_OR_EQUAL, s);
    d = ui.deEnd->date();
    s = d.toString("yyyyMMdd");
    //tf.addFilterExpr(TableFilter::F_REC_RDATE, TableFilter::OP_LOWER_OR_EQUAL, s);
	
    //browseReceipes->setFilter(&tf);
}


void DlgBill2::slotCancel()
{
    res = RES_CANCEL;
    this->reject();
}


void DlgBill2::slotBack()
{
    res = RES_BACK;
    this->reject();
}


void DlgBill2::slotSave()
{
    res = RES_SAVE;
    this->accept();
}


void DlgBill2::slotUpdate(int a)
{
    if (a != myDB::TABLE_RECEIPES) return;
    float f;
    int num;
    QString s;
	
    num = db.getNumOfBilledReceipes(ui.deStart->date(), ui.deEnd->date(), &f);
    ui.leNumRec->setText(QString::number(num));
    s.asprintf("%.2f", f);
    ui.leSum->setText(s);
}


void DlgBill2::slotBillSelected()
{	
}


void DlgBill2::slotBillAll()
{	
}
