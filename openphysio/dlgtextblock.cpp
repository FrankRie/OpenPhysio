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

#include "dlgtextblock.h"
#include "ui_dlgtextblock.h"
#include "config.h"

DlgTextBlock::DlgTextBlock(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgTextBlock)
{
    ui->setupUi(this);

    connect(ui->pbCancel, &QPushButton::clicked, this, &DlgTextBlock::reject);
    connect(ui->pbSave, &QPushButton::clicked, this, &DlgTextBlock::accept);
    connect(ui->listWidget, &QListWidget::doubleClicked, this, &DlgTextBlock::slotSelectText);

    ui->cbAddDate->setChecked((bool)myConf.get(ConfXML::AREA_TEXT_MODULES, "autodate").toInt());
}


DlgTextBlock::~DlgTextBlock()
{
    delete ui;
    while (!listTextModules.isEmpty()) delete listTextModules.takeFirst();
}


void DlgTextBlock::init(int domain)
{
    db.retrieveItemList(myDB::TABLE_TEXT_MODULES, reinterpret_cast<QList<cTableItem*>&>(listTextModules));

    QStringList l;
    for (int i = 0; i < listTextModules.count(); i++) {
        cTextModule *ctm = listTextModules.at(i);
        if (ctm->getDomain() == domain) {
            l << ctm->getText();
        }
    }
    ui->listWidget->addItems(l);
}


QString DlgTextBlock::getTextModule()
{
    QListWidgetItem *w = ui->listWidget->currentItem();
    if (w == NULL) return "";
    QString s = w->text();
    if (s.isEmpty()) return s;
    if (ui->cbAddDate->isChecked()) {
        s = "[" + QDate::currentDate().toString("dd.MM.yyyy") + "] " + s;
    }
    return s;
}


void DlgTextBlock::slotSelectText()
{
    this->accept();
}
