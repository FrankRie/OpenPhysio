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

#include "dlgschedule.h"
#include "ui_dlgschedule.h"
#include "helperwrapper.h"
#include "msgbox.h"
#include "employeetools.h"
#include "gendoc.h"


DlgSchedule::DlgSchedule(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgSchedule)
{
    ui->setupUi(this);

    connect(ui->pbCancel, &QPushButton::clicked, this, &DlgSchedule::slotCancel);
    connect(ui->pbCreate, &QPushButton::clicked, this, &DlgSchedule::slotCreate);
    connect(ui->cbMode, &QComboBox::currentIndexChanged, this, &DlgSchedule::slotMode);
    installEventFilter(this);
}


DlgSchedule::~DlgSchedule()
{
    delete ui;
    while (!list_empl.isEmpty()) delete list_empl.takeFirst();
}


void DlgSchedule::init(QDate start_date, QDate end_date, QTime start_time, QTime end_time)
{
    ui->cbMode->setCurrentIndex(0);
    ui->deTo->setEnabled(false);
    ui->deFrom->setDate(start_date);
    ui->deTo->setDate(end_date);
    ui->teFrom->setTime(start_time);
    ui->teTo->setTime(end_time);
    EmplTool.getActiveEmployees(list_empl, cEmployeeGroup::GROUP_THERAPY);
    EmplTool.fillComboBox(ui->cbEmployees, cEmployeeGroup::GROUP_THERAPY);
}


bool DlgSchedule::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        if (ke->key() == Qt::Key_F1 || ke->key() == Qt::Key_Help) {
            helpWrapper.showHelpForKeyword("openphysio_schedule", true);
            return true;
        }
    }
    return QDialog::eventFilter(obj, event);
}


void DlgSchedule::slotCancel()
{
    this->reject();
}


void DlgSchedule::slotCreate()
{
    int idx = ui->cbEmployees->currentIndex();
    if (idx < 0) return;
    if (idx >= list_empl.count()) return;

    GenDoc gendoc;
    QDate date_end;
    if (ui->cbMode->currentIndex() == 0) date_end = ui->deFrom->date();
    else date_end = ui->deTo->date();
    gendoc.genSchedule(*list_empl.at(idx), ui->cbMode->currentIndex(),
                       ui->deFrom->date(), date_end, ui->teFrom->time(), ui->teTo->time());
}


void DlgSchedule::slotMode(int m)
{
    if (m == 0) ui->deTo->setEnabled(false);
    else ui->deTo->setEnabled(true);
}
