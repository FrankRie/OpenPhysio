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

#include "dlgmanagecars.h"
#include "ui_dlgmanagecars.h"
#include "msgbox.h"
#include "helperwrapper.h"


DlgManageCars::DlgManageCars(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgManageCars)
{
    ui->setupUi(this);
    flag_change = false;

    connect(ui->pbAdd, &QPushButton::clicked, this, &DlgManageCars::slotAddCar);
    connect(ui->pbDel, &QPushButton::clicked, this, &DlgManageCars::slotDelCar);
    connect(ui->pbCancel, &QPushButton::clicked, this, &DlgManageCars::slotCancel);
    connect(ui->pbSave, &QPushButton::clicked, this, &DlgManageCars::slotSave);
    ui->tableWidgetCar->hideColumn(0);

    loadTable();
    installEventFilter(this);
}


DlgManageCars::~DlgManageCars()
{
    while (!cars.isEmpty()) delete cars.takeFirst();
    delete ui;
}


void DlgManageCars::loadTable()
{
    disconnect(ui->tableWidgetCar, &QTableWidget::cellChanged, this, &DlgManageCars::slotModified);

    while (ui->tableWidgetCar->rowCount() > 0) ui->tableWidgetCar->removeRow(0);

    QAbstractItemModel* model;
    db.retrieveItemList(myDB::TABLE_CARS, cars);
    model = ui->tableWidgetCar->model();

    for (int i = 0; i < cars.count(); i++) {
        cCar *c = (cCar*)(cars.at(i));
        ui->tableWidgetCar->insertRow(i);
        model->setData(model->index(i, 0), c->getID());
        model->setData(model->index(i, 1), c->getName());
        model->setData(model->index(i, 2), c->getNotice());
    }

    ui->tableWidgetCar->resizeColumnsToContents();
    connect(ui->tableWidgetCar, &QTableWidget::cellChanged, this, &DlgManageCars::slotModified);
}


void DlgManageCars::slotModified()
{
    flag_change = true;
}


void DlgManageCars::slotAddCar()
{
    int r = ui->tableWidgetCar->rowCount();
    ui->tableWidgetCar->insertRow(r);
    QAbstractItemModel* model;
    model = ui->tableWidgetCar->model();

    // set ID of new groups to -1
    model->setData(model->index(r, 0), -1);
    flag_change = true;
}


void DlgManageCars::slotDelCar()
{
    if (ui->tableWidgetCar->rowCount() > 0) {
        int i = ui->tableWidgetCar->currentRow();
        if (i >= 0) {
            QAbstractItemModel* model;
            model = ui->tableWidgetCar->model();
            int id = model->data(model->index(i, 0)).toInt();
            if (id >= 0) { // mark ID of existing group to be deleted
                for (int x = 0; x < cars.count(); x++) {
                    cCar *c = (cCar*)(cars.at(x));
                    if (c->getID() == id) {
                        c->setFlag(cTableItem::FLAG_DEL);
                        break;
                    }
                }
            }
            ui->tableWidgetCar->removeRow(i);
            flag_change = true;
        }
    }
}


void DlgManageCars::slotSave()
{
    QAbstractItemModel* model;
    model = ui->tableWidgetCar->model();
    for (int i = 0; i < ui->tableWidgetCar->rowCount(); i++) {
        int id = model->data(model->index(i, 0)).toInt();
        if (id >= 0) { // search for ID in car list and update the corresponding entry
            for (int x = 0; x < cars.count(); x++) {
                cCar *c = (cCar*)(cars.at(x));
                if (id == c->getID()) {
                    c->setFlag(cTableItem::FLAG_MODIFIED);
                    c->setName(model->data(model->index(i, 1)).toString());
                    c->setNotice(model->data(model->index(i, 2)).toString());
                    break;
                }
            }
        } else if (id == -1) { // we have to append a new car entry
            cars.append(new cCar());
            cCar *c = (cCar*)(cars.at(cars.count() - 1));
            c->setFlag(cTableItem::FLAG_NEW);
            c->setName(model->data(model->index(i, 1)).toString());
            c->setNotice(model->data(model->index(i, 2)).toString());
        }
    }

    if (db.saveItemList(myDB::TABLE_CARS, cars)) {
        flag_change = false;
        this->accept();
    } else QMessageBox::warning(NULL, tr("OpenPhysio - Hinweis"), "Fehler beim Speichern aufgetreten!", QMessageBox::Ok);
}


void DlgManageCars::slotCancel()
{
    if (flag_change) {
        MsgBox msgBox;
        if (msgBox.execYesNoBox(QString::fromUtf8("Wollen Sie die Änderungen vorher speichern?"), MsgBox::BTN_YES) == MsgBox::BTN_YES) {
            slotSave();
            return;
        }
    }
    this->reject();
}


bool DlgManageCars::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        if (ke->key() == Qt::Key_F1 || ke->key() == Qt::Key_Help) {
            helpWrapper.showHelpForKeyword("openphysio_cars", true);
            return true;
        }
    }
    return QDialog::eventFilter(obj, event);
}
