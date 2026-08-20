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

#include "dlgreceipeprof.h"
#include "ui_dlgreceipeprof.h"
#include "mydb.h"


DlgReceipeProf::DlgReceipeProf(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgReceipeProf)
{
    ui->setupUi(this);
    connect(ui->pbCancel, &QPushButton::clicked, this, &DlgReceipeProf::slotCancel);
    connect(ui->pbPhysio, &QPushButton::clicked, this, &DlgReceipeProf::slotPhysio);
    connect(ui->pbErgo, &QPushButton::clicked, this, &DlgReceipeProf::slotErgo);
    connect(ui->pbLogo, &QPushButton::clicked, this, &DlgReceipeProf::slotLogo);

    cSurgery surg = db.getSurgery();
    int prof = surg.getProfession();
    if (!(prof & 1)) ui->pbPhysio->hide();
    if (!((prof >> 1 ) & 1)) ui->pbErgo->hide();
    if (!((prof >> 2 ) & 1)) ui->pbLogo->hide();
}


DlgReceipeProf::~DlgReceipeProf()
{
    delete ui;
}


void DlgReceipeProf::slotPhysio()
{
    this->done(0);
}


void DlgReceipeProf::slotErgo()
{
    this->done(1);
}


void DlgReceipeProf::slotLogo()
{
    this->done(2);
}


void DlgReceipeProf::slotCancel()
{
    this->done(-1);
}
