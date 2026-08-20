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

#include "dlghelp.h"
#include "ui_dlghelp.h"


DlgHelp::DlgHelp(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::DlgHelp)
{
    m_ui->setupUi(this);
    helperWidget = new HelperWidget();
    ((QGridLayout*)(this->layout()))->addWidget(helperWidget, 1, 0);

    connect(m_ui->pbPrev, &QPushButton::clicked, helperWidget, &HelperWidget::backward);
    connect(m_ui->pbNext, &QPushButton::clicked, helperWidget, &HelperWidget::forward);
}


DlgHelp::~DlgHelp()
{
    delete m_ui;
    if (helperWidget != NULL) delete helperWidget;
}


void DlgHelp::setKeyword(const QString &keyword)
{
    if (helperWidget != NULL) helperWidget->showHelpForKeyword(keyword);
}
