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

#include "dlgdatemove.h"
#include "ui_dlgdatemove.h"
#include "msgbox.h"
#include "employeetools.h"
#include "helperwrapper.h"


DlgDateMove::DlgDateMove(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgDateMove)
{
    ui->setupUi(this);
    dateEdit = new DateEdit();
    timeEdit = new TimeEdit();
    QGridLayout *grid = qobject_cast<QGridLayout*>(this->layout());
    if (grid) {
        grid->addWidget(dateEdit, 1, 1);
        grid->addWidget(timeEdit, 2, 1);
    }
    connect(ui->pbCancel, &QPushButton::clicked, this, &DlgDateMove::slotCancel);
    connect(ui->pbSave, &QPushButton::clicked, this, &DlgDateMove::slotSave);
    installEventFilter(this);
}


DlgDateMove::~DlgDateMove()
{
    delete ui;
}


void DlgDateMove::init(cDateItem& cdi)
{
    new_cdi = cdi;
    QString s = "Vom " + cdi.getDate().toString("dd.MM.yyyy") + " um " + cdi.getStart().toString("hh:mm") + " Uhr ";
    s = s + "(" + EmplTool.getEmplPID(cdi.getEID()) + ") auf:";
    ui->labFrom->setText(s);
    dateEdit->setDate(cdi.getDate());
    timeEdit->setTime(cdi.getStart());
    ui->comboBox->clear();
    int idx = -1;
    for (int i = 0; i < db.listEmpl.count(); i++) {
        ui->comboBox->addItem(db.listEmpl.at(i)->getFullName());
        if (db.listEmpl.at(i)->getID() == cdi.getEID()) idx = i;
    }
    ui->comboBox->setCurrentIndex(idx);
}


void DlgDateMove::slotCancel()
{
    reject();
}


void DlgDateMove::slotSave()
{
    int idx = ui->comboBox->currentIndex();
    if (idx < 0 || idx >= db.listEmpl.count()) return;

    if (!dateEdit->date().isValid()) {
        MsgBox::error(QString::fromUtf8("Ungültiges Datum!"));
        return;
    }
    if (!timeEdit->time().isValid()) {
        MsgBox::error(QString::fromUtf8("Ungültige Uhrzeit!"));
        return;
    }

    int duration = new_cdi.getStart().secsTo(new_cdi.getEnd());
    new_cdi.setDate(dateEdit->date());
    new_cdi.setStart(timeEdit->time());
    new_cdi.setEnd(new_cdi.getStart().addSecs(duration));
    new_cdi.setEID(db.listEmpl.at(idx)->getID());

    DateTools dt;
    if (dt.OverlapCheck(new_cdi)) {
        if (db.updateItem(new_cdi)) accept();
        else MsgBox::error(QString::fromUtf8("Es ist ein Fehler beim Speichern aufgetreten!"));
    }
}


bool DlgDateMove::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        if (ke->key() == Qt::Key_F1 || ke->key() == Qt::Key_Help) {
            helpWrapper.showHelpForKeyword("openphysio_date_move", true);
            return true;
        }
    }
    return QDialog::eventFilter(obj, event);
}
