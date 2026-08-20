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

#include "dlgmanagetherapygroups.h"
#include "ui_dlgmanagetherapygroups.h"
#include "msgbox.h"
#include "helperwrapper.h"


DlgManageTherapyGroups::DlgManageTherapyGroups(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgManageTherapyGroups)
{
    ui->setupUi(this);
    flag_change = false;

    connect(ui->pbAdd, &QPushButton::clicked, this, &DlgManageTherapyGroups::slotAddGroup);
    connect(ui->pbDel, &QPushButton::clicked, this, &DlgManageTherapyGroups::slotDelGroup);
    connect(ui->pbCancel, &QPushButton::clicked, this, &DlgManageTherapyGroups::slotCancel);
    connect(ui->pbSave, &QPushButton::clicked, this, &DlgManageTherapyGroups::slotSave);
    ui->tableWidgetGroup->hideColumn(0);

    loadTable();
    installEventFilter(this);
}


DlgManageTherapyGroups::~DlgManageTherapyGroups()
{
    while (!groups.isEmpty()) delete groups.takeFirst();
    delete ui;
}

void DlgManageTherapyGroups::loadTable()
{
    disconnect(ui->tableWidgetGroup, &QTableWidget::cellChanged, this, &DlgManageTherapyGroups::slotModified);

    while (ui->tableWidgetGroup->rowCount() > 0) ui->tableWidgetGroup->removeRow(0);

    QAbstractItemModel* model;
    db.retrieveItemList(myDB::TABLE_THERAPY_GROUPS, groups);
    model = ui->tableWidgetGroup->model();

    for (int i = 0; i < groups.count(); i++) {
        cTherapyGroup *g = (cTherapyGroup*)(groups.at(i));
        ui->tableWidgetGroup->insertRow(i);
        model->setData(model->index(i, 0), g->getID());
        model->setData(model->index(i, 1), g->getName());
        model->setData(model->index(i, 2), g->getDescription());
    }

    ui->tableWidgetGroup->resizeColumnsToContents();
    connect(ui->tableWidgetGroup, &QTableWidget::cellChanged, this, &DlgManageTherapyGroups::slotModified);
}

void DlgManageTherapyGroups::slotModified()
{
    flag_change = true;
}


void DlgManageTherapyGroups::slotAddGroup()
{
    int r = ui->tableWidgetGroup->rowCount();
    ui->tableWidgetGroup->insertRow(r);
    QAbstractItemModel* model;
    model = ui->tableWidgetGroup->model();

    // set ID of new groups to -1
    model->setData(model->index(r, 0), -1);
    flag_change = true;
}


void DlgManageTherapyGroups::slotDelGroup()
{
    if (ui->tableWidgetGroup->rowCount() > 0) {
        int i = ui->tableWidgetGroup->currentRow();
        if (i >= 0) {
            QAbstractItemModel* model;
            model = ui->tableWidgetGroup->model();
            int id = model->data(model->index(i, 0)).toInt();
            if (id >= 0) { // mark ID of existing group to be deleted
                for (int x = 0; x < groups.count(); x++) {
                    cTherapyGroup *g = (cTherapyGroup*)(groups.at(x));
                    if (g->getID() == id) {
                        g->setFlag(cTableItem::FLAG_DEL);
                        break;
                    }
                }
            }
            ui->tableWidgetGroup->removeRow(i);
            flag_change = true;
        }
    }
}


void DlgManageTherapyGroups::slotSave()
{
    QAbstractItemModel* model;
    model = ui->tableWidgetGroup->model();
    for (int i = 0; i < ui->tableWidgetGroup->rowCount(); i++) {
        int id = model->data(model->index(i, 0)).toInt();
        if (id >= 0) { // search for ID in group list and update the corresponding entry
            for (int x = 0; x < groups.count(); x++) {
                cTherapyGroup *g = (cTherapyGroup*)(groups.at(x));
                if (id == g->getID()) {
                    g->setFlag(cTableItem::FLAG_MODIFIED);
                    g->setName(model->data(model->index(i, 1)).toString());
                    g->setDescription(model->data(model->index(i, 2)).toString());
                    break;
                }
            }
        } else if (id == -1) { // we have to append a new group entry
            groups.append(new cTherapyGroup());
            cTherapyGroup *g = (cTherapyGroup*)(groups.at(groups.count() - 1));
            g->setFlag(cTableItem::FLAG_NEW);
            g->setName(model->data(model->index(i, 1)).toString());
            g->setDescription(model->data(model->index(i, 2)).toString());
        }
    }

    if (db.saveItemList(myDB::TABLE_THERAPY_GROUPS, groups)) {
        flag_change = false;
        this->accept();
    } else QMessageBox::warning(NULL, tr("OpenPhysio - Hinweis"), "Fehler beim Speichern aufgetreten!", QMessageBox::Ok);
}


void DlgManageTherapyGroups::slotCancel()
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


bool DlgManageTherapyGroups::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        if (ke->key() == Qt::Key_F1 || ke->key() == Qt::Key_Help) {
            helpWrapper.showHelpForKeyword("openphysio_therapygroups", true);
            return true;
        }
    }
    return QDialog::eventFilter(obj, event);
}
