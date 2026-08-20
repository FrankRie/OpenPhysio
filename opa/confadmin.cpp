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

#include "confadmin.h"
#include "ui_confadmin.h"


ConfAdmin::ConfAdmin(QWidget *parent, int l, int a) :
    ConfBase(parent, l, a),
    ui(new Ui::ConfAdmin)
{
    ui->setupUi(this);
    setTitle(QString::fromUtf8("Admin-Zugang"));
    setKeyword("opa_admin");
}


ConfAdmin::~ConfAdmin()
{
    delete ui;
}


void ConfAdmin::init()
{
    connect(ui->lePW1, &QLineEdit::textChanged, this, &ConfAdmin::slotCheckPass);
    connect(ui->lePW2, &QLineEdit::textChanged, this, &ConfAdmin::slotCheckPass);
    QByteArray qba = myConf.get(ConfXML::AREA_ADMIN, "access").toUtf8();// toAscii();
    QString pw_decoded = "";
    if (!qba.isEmpty()) pw_decoded = QByteArray::fromBase64(qba);
    ui->lePW1->setText(pw_decoded);
    ui->lePW2->setText(pw_decoded);
}


bool ConfAdmin::write()
{
    if (ui->lePW1->text() != ui->lePW2->text()) return false;
    myConf.set(ConfXML::AREA_ADMIN, "access", ui->lePW1->text().toUtf8() /*Ascii()*/.toBase64());
    return true;
}


void ConfAdmin::slotCheckPass()
{
    QPalette p_hint(QApplication::palette());
    QPalette p_normal(QApplication::palette());
    p_hint.setColor(QPalette::Base, Qt::yellow);
    if (ui->lePW1->text() != ui->lePW2->text()) {
        ui->lePW2->setPalette(p_hint);
    } else {
        ui->lePW2->setPalette(p_normal);
    }
}
