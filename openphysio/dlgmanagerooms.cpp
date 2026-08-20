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

#include "dlgmanagerooms.h"
#include "ui_dlgmanagerooms.h"
#include "msgbox.h"
#include "helperwrapper.h"


DlgManageRooms::DlgManageRooms(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgManageRooms)
{
    ui->setupUi(this);
    flag_change = false;

    connect(ui->pbAdd, &QPushButton::clicked, this, &DlgManageRooms::slotAddRoom);
    connect(ui->pbDel, &QPushButton::clicked, this, &DlgManageRooms::slotDelRoom);
    connect(ui->pbCancel, &QPushButton::clicked, this, &DlgManageRooms::slotCancel);
    connect(ui->pbSave, &QPushButton::clicked, this, &DlgManageRooms::slotSave);
    ui->tableWidgetRoom->hideColumn(0);

    loadTable();
    installEventFilter(this);
}


DlgManageRooms::~DlgManageRooms()
{
    while (!rooms.isEmpty()) delete rooms.takeFirst();
    delete ui;
}


void DlgManageRooms::loadTable()
{
    disconnect(ui->tableWidgetRoom, &QTableWidget::cellChanged, this, &DlgManageRooms::slotModified);

    while (ui->tableWidgetRoom->rowCount() > 0) ui->tableWidgetRoom->removeRow(0);
    db.retrieveItemList(myDB::TABLE_ROOMS, rooms);
    QAbstractItemModel* model;
    model = ui->tableWidgetRoom->model();

    for (int i = 0; i < rooms.count(); i++) {
        cRoom *r = (cRoom*)(rooms.at(i));
        ui->tableWidgetRoom->insertRow(i);
        model->setData(model->index(i, 0), r->getID());
        model->setData(model->index(i, 1), r->getName());
        model->setData(model->index(i, 2), r->getBuilding());
        model->setData(model->index(i, 3), r->getNotice());
    }

    ui->tableWidgetRoom->resizeColumnsToContents();

    connect(ui->tableWidgetRoom, &QTableWidget::cellChanged, this, &DlgManageRooms::slotModified);
}


void DlgManageRooms::slotModified()
{
    flag_change = true;
}


void DlgManageRooms::slotAddRoom()
{
    int r = ui->tableWidgetRoom->rowCount();
    ui->tableWidgetRoom->insertRow(r);
    QAbstractItemModel* model;
    model = ui->tableWidgetRoom->model();

    // set ID of new rooms to -1
    model->setData(model->index(r, 0), -1);
    flag_change = true;
}


void DlgManageRooms::slotDelRoom()
{
    if (ui->tableWidgetRoom->rowCount() > 0) {
        int i = ui->tableWidgetRoom->currentRow();
        if (i >= 0) {
            QAbstractItemModel* model;
            model = ui->tableWidgetRoom->model();
            int id = model->data(model->index(i, 0)).toInt();
            if (id >= 0) { // mark ID of existing room to be deleted
                for (int x = 0; x < rooms.count(); x++) {
                    cRoom *r = (cRoom*)(rooms.at(x));
                    if (r->getID() == id) {
                        r->setFlag(cTableItem::FLAG_DEL);
                        break;
                    }
                }
            }
            ui->tableWidgetRoom->removeRow(i);
            flag_change = true;
        }
    }
}


void DlgManageRooms::slotSave()
{
    QAbstractItemModel* model;
    model = ui->tableWidgetRoom->model();

    for (int i = 0; i < ui->tableWidgetRoom->rowCount(); i++) {
        int id = model->data(model->index(i, 0)).toInt();
        if (id >= 0) { // search for ID in room list and update the corresponding entry
            for (int x = 0; x < rooms.count(); x++) {
                cRoom *r = (cRoom*)(rooms.at(x));
                if (id == r->getID()) {
                    r->setFlag(cTableItem::FLAG_MODIFIED);
                    r->setName(model->data(model->index(i, 1)).toString());
                    r->setBuilding(model->data(model->index(i, 2)).toString());
                    r->setNotice(model->data(model->index(i, 3)).toString());
                    break;
                }
            }
        } else if (id == -1) { // we have to append a new room entry
            rooms.append(new cRoom());
            cRoom *r = (cRoom*)(rooms.at(rooms.count() - 1));
            r->setFlag(cTableItem::FLAG_NEW);
            r->setName(model->data(model->index(i, 1)).toString());
            r->setBuilding(model->data(model->index(i, 2)).toString());
            r->setNotice(model->data(model->index(i, 3)).toString());
        }
    }
    if (db.saveItemList(myDB::TABLE_ROOMS, rooms)) {
        flag_change = false;
        this->accept();
    } else QMessageBox::warning(NULL, tr("OpenPhysio - Hinweis"), "Fehler beim Speichern aufgetreten!", QMessageBox::Ok);
}


void DlgManageRooms::slotCancel()
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


bool DlgManageRooms::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        if (ke->key() == Qt::Key_F1 || ke->key() == Qt::Key_Help) {
            helpWrapper.showHelpForKeyword("openphysio_rooms", true);
            return true;
        }
    }
    return QDialog::eventFilter(obj, event);
}
