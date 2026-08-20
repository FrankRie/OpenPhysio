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

#include "browsecash.h"
#include "ui_browsecash.h"
#include "config.h"


BrowseCash::BrowseCash(int mode, QWidget *parent) :
    Browse(mode, parent),
    m_ui(new Ui::BrowseCash)
{
    m_ui->setupUi(this);
    init((TableViewAnimated*)m_ui->tvCash, myDB::TABLE_CASH);

    // add display filter widget
    QGridLayout *layout = (QGridLayout*) this->layout();
    layout->addWidget(&displayFilter, 0, 0);
}


BrowseCash::~BrowseCash()
{
    delete m_ui;
}

