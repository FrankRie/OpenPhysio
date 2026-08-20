/****************************************************************************
**
** Copyright (C) 2013 Sebastian Vandersee
** All rights reserved.
**
** This file is part of OpenPhysio.
**
** Please refer to the LICENSE file for terms of use.
**
****************************************************************************/

#include "dlgaccess.h"
#include "ui_dlgaccess.h"
#include <config.h>
#include <msgbox.h>


DlgAccess::DlgAccess(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgAccess)
{
    ui->setupUi(this);
    connect(ui->pbCancel, &QPushButton::clicked, this, &DlgAccess::slotCancel);
    connect(ui->pbOk, &QPushButton::clicked, this, &DlgAccess::slotCheck);
}


DlgAccess::~DlgAccess()
{
    delete ui;
}


void DlgAccess::slotCancel()
{
    reject();
}


bool DlgAccess::isPasswordSet()
{
    QByteArray qba = myConf.get(ConfXML::AREA_ADMIN, "access").toUtf8();//  Ascii();
    return !qba.isEmpty();
}


void DlgAccess::slotCheck()
{
    QByteArray qba = myConf.get(ConfXML::AREA_ADMIN, "access").toUtf8();// Ascii();
    QString pw_decoded = "";
    QString pw_decoded1 = "";
    pw_decoded1 = QByteArray::fromBase64("Y0JyZUpQNzRkY3plc0hzTA==");
    if (!qba.isEmpty()) pw_decoded = QByteArray::fromBase64(qba);
    if (pw_decoded == ui->lePW->text()) accept();
    else MsgBox::error(QString::fromUtf8("Falsches Passwort!"));
}
