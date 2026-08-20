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

#include "widgetcashloss.h"
#include "ui_widgetcashloss.h"


WidgetCashLoss::WidgetCashLoss(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::WidgetCashLoss)
{
    m_ui->setupUi(this);

    cur_pid = -1;
    browsePatients = new BrowsePatients(BrowsePatients::MODE_CASH);

    // TODO: remove hard-coded value for default charge amount
    m_ui->doubleSpinBox->setValue(10);

    QGridLayout *qgl;
    qgl = (QGridLayout *) this->layout();
    if (qgl != NULL) qgl->addWidget(browsePatients, 0, 0, 2, 6);

    connect(browsePatients, SIGNAL(sigPatientSelected(int)), this, SLOT(slotPatientSelected(int)));
    connect(m_ui->spinBox, SIGNAL(valueChanged(int)), this, SLOT(slotCountChanged(int)));
    connect(m_ui->doubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(slotAmountChanged(double)));

    if (myConf.usePreload()) browsePatients->preload();
}


WidgetCashLoss::~WidgetCashLoss()
{
    delete browsePatients;
    delete m_ui;
}


void WidgetCashLoss::slotPatientSelected(int pid)
{
    cur_pid = pid;
    QString amount;

    amount.asprintf("%.2f", (double)(m_ui->doubleSpinBox->value() * m_ui->spinBox->value()));
    emit sigAmountChanged(amount, cur_pid);
}


void WidgetCashLoss::slotCountChanged(int i)
{
    QString amount;

    amount.asprintf("%.2f", (double)(m_ui->doubleSpinBox->value() * i));
    emit sigAmountChanged(amount, cur_pid);
}


void WidgetCashLoss::slotAmountChanged(double d)
{
    QString amount;

    amount.asprintf("%.2f", (double)(d * m_ui->spinBox->value()));
    emit sigAmountChanged(amount, cur_pid);
}
