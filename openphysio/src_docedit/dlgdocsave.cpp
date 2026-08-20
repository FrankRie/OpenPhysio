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

#include "dlgdocsave.h"
#include "ui_dlgdocsave.h"


DlgDocSave::DlgDocSave(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgDocSave)
{
    ui->setupUi(this);
    connect(ui->pbCancel, &QPushButton::clicked, this, &DlgDocSave::reject);
    connect(ui->pbSave, &QPushButton::clicked, this, &DlgDocSave::slotSave);
}


DlgDocSave::~DlgDocSave()
{
    delete ui;
}


void DlgDocSave::init(QString tmpl_name)
{
    ui->leName->setText(tmpl_name);
}


void DlgDocSave::slotSave()
{
    if (ui->leName->text().isEmpty()) {
        QMessageBox::warning(this, "OpenPhysio", QString::fromUtf8("Es wurde kein gültiger Name angegeben!"));
    } else {
        accept();
    }
}


QString DlgDocSave::getTemplateName()
{
    return ui->leName->text();
}
