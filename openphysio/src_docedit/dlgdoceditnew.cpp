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

#include "dlgdoceditnew.h"
#include "ui_dlgdoceditnew.h"


DlgDocEditNew::DlgDocEditNew(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgDocEditNew)
{
    ui->setupUi(this);
    connect(ui->pbCancel, &QPushButton::clicked, this, &DlgDocEditNew::reject);
    connect(ui->pbSave, &QPushButton::clicked, this, &DlgDocEditNew::slotAccept);
    installEventFilter(this);
}


DlgDocEditNew::~DlgDocEditNew()
{
    delete ui;
}


bool DlgDocEditNew::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        if (ke->key() == Qt::Key_F1 || ke->key() == Qt::Key_Help) {
            //helpWrapper.showHelpForKeyword("", true);
            return true;
        }
    }
    return QDialog::eventFilter(obj, event);
}


void DlgDocEditNew::init(QStringList l)
{
    ui->cbDocType->clear();
    ui->cbDocType->addItems(l);
}


QString DlgDocEditNew::getSelectedDocType()
{
    return ui->cbDocType->currentText();
}


int DlgDocEditNew::getSelectedPageSize()
{
    return ui->cbPageSize->currentIndex();
}


QString DlgDocEditNew::getName()
{
    return ui->leName->text();
}


void DlgDocEditNew::slotAccept()
{
    if (ui->leName->text().isEmpty()) {
        QMessageBox::warning(this, "OpenPhysio", QString::fromUtf8("Sie haben noch keinen Namen eingegeben!"));
    } else {
        accept();
    }
}
