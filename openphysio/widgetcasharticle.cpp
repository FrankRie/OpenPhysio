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

#include "widgetcasharticle.h"
#include "ui_widgetcasharticle.h"


WidgetCashArticle::WidgetCashArticle(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::WidgetCashArticle)
{
    m_ui->setupUi(this);
}


WidgetCashArticle::~WidgetCashArticle()
{
    delete m_ui;
}
