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

#include "confother.h"
#include "ui_confother.h"


ConfOther::ConfOther(QWidget *parent, int l, int a) :
    ConfBase(parent, l, a),
    ui(new Ui::ConfOther)
{
    ui->setupUi(this);
    setTitle("Sonstiges");
    setKeyword("opa_other");
}


ConfOther::~ConfOther()
{
    delete ui;
}

void ConfOther::init()
{
    if (myConf.usePreload()) ui->cbPreload->setChecked(true);
    else ui->cbPreload->setChecked(false);
}

bool ConfOther::write()
{
    if (ui->cbPreload->checkState() == Qt::Checked) myConf.set(ConfXML::AREA_OTHER, "preload", "1");
    else myConf.set(ConfXML::AREA_OTHER, "preload", "0");
    return true;
}
