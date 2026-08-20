/****************************************************************************
**
** Copyright (C) 2013 Sebastian Vandersee
** All rights reserved.
**
** This file is part of OpenPhysio.
**
** Please refer to the LICENSE file for terms of use.
**
****************************************************************************/

#include "confshortcuts.h"
#include "ui_confshortcuts.h"
#include "msgbox.h"


ConfShortcuts::ConfShortcuts(QWidget *parent, int l, int a) :
    ConfBase(parent, l, a),
    ui(new Ui::ConfShortcuts)
{
    ui->setupUi(this);
    setTitle(QString::fromUtf8("Rezept-Shortcuts"));
    setKeyword("opa_shortcuts");

#ifdef Q_OS_MACX
    ui->widget->layout()->setSpacing(12);
#endif

    connect(ui->pbAdd, &QPushButton::clicked, this, &ConfShortcuts::slotAdd);
    connect(ui->pbDel, &QPushButton::clicked, this, &ConfShortcuts::slotDel);
    connect(ui->pbUp, &QPushButton::clicked, this, &ConfShortcuts::slotUp);
    connect(ui->pbDown, &QPushButton::clicked, this, &ConfShortcuts::slotDown);
    curProf = -1;
}


ConfShortcuts::~ConfShortcuts()
{
    delete ui;
    for (int i = 0; i < list_prof.count(); i++) delete list_prof.at(i);
}


void ConfShortcuts::init()
{
    disconnect(ui->cbProfession, &QComboBox::currentIndexChanged, this, &ConfShortcuts::slotSelectProfession);
    flag_change = false;
    db.retrieveItemList(myDB::TABLE_PROFESSIONS, list_prof);
    for (int i = 0; i < list_prof.count(); i++)
        ui->cbProfession->addItem(((cProfession*)(list_prof.at(i)))->getName());

    curProf = 0;
    loadTable(0);
    connect(ui->cbProfession, &QComboBox::currentIndexChanged, this, &ConfShortcuts::slotSelectProfession);
}


bool ConfShortcuts::write()
{
    if (flag_change) {
        // mark existing entries for deletion
        for (int i = 0; i < listReceipeShortcuts.count(); i++)
            listReceipeShortcuts.at(i)->setFlag(cTableItem::FLAG_DEL);
        // in this case: save list = delete all entries
        db.saveItemList(myDB::TABLE_RECEIPE_SHORTCUTS, listReceipeShortcuts);
        listReceipeShortcuts.clear();

        QAbstractItemModel* model;
        QString strAmount, strPosNr, strDescr;

        model = ui->tableShortcuts->model();
        for (int i = 0; i < ui->tableShortcuts->rowCount(); i++) {
            strAmount = model->index(i, 0).data().toString();
            strDescr = model->index(i, 1).data().toString();
            strPosNr = model->index(i, 2).data().toString();
            if (!strAmount.isEmpty() && (!strDescr.isEmpty() && (!strPosNr.isEmpty()))) {
                listReceipeShortcuts.append(new cReceipeShortcut());
                cReceipeShortcut *cut = (cReceipeShortcut*)listReceipeShortcuts.last();
                cut->setAmount(strAmount.toInt());
                cut->setDescription(strDescr);
                cut->setPosNr(strPosNr);
                cut->setProfession(curProf);
            }
        }
        // finally, write the items (shortcuts) into the database
        if (listReceipeShortcuts.count() > 0)
            db.saveItemList(myDB::TABLE_RECEIPE_SHORTCUTS, listReceipeShortcuts);

        flag_change = false;
    }
    return true;
}


void ConfShortcuts::loadTable(int prof)
{
    disconnect(ui->tableShortcuts, &QTableWidget::cellChanged, this, &ConfShortcuts::slotModified);

    flag_change = false;
    ui->tableShortcuts->clearContents();
    while (ui->tableShortcuts->rowCount() > 0)
        ui->tableShortcuts->removeRow(0);

    QAbstractItemModel* model;
    model = ui->tableShortcuts->model();
    // we need to load the shortcuts on our own since opa did not invoke loadBaseTypes() of DB layer
    db.retrieveItemList(myDB::TABLE_RECEIPE_SHORTCUTS, listReceipeShortcuts);

    // only use the shortcuts that are related to the selectedd profession
    int co = listReceipeShortcuts.count();
    if (co > 0) {
        int i = 0;
        while (true) {
            cReceipeShortcut *cut = (cReceipeShortcut*)listReceipeShortcuts.at(i);
            if (cut->getProfession() != prof) {
                delete listReceipeShortcuts.takeAt(i);
            } else i++;
            if (i >= listReceipeShortcuts.count()) break;
        }
    }

    co = listReceipeShortcuts.count();
    if (co > 0) {
        for (int i = co  - 1; i >= 0; i--) {
            ui->tableShortcuts->insertRow(0);
            cReceipeShortcut *cut = (cReceipeShortcut*)listReceipeShortcuts.at(i);
            model->setData(model->index(0, 0), cut->getAmount());
            model->setData(model->index(0, 1), cut->getDescription());
            model->setData(model->index(0, 2), cut->getPosNr());
        }
    }
    ui->tableShortcuts->resizeColumnsToContents();

    connect(ui->tableShortcuts, &QTableWidget::cellChanged, this, &ConfShortcuts::slotModified);
}


void ConfShortcuts::slotAdd()
{
    ui->tableShortcuts->insertRow(ui->tableShortcuts->rowCount());
}


void ConfShortcuts::slotDel()
{
    if (ui->tableShortcuts->rowCount() > 0) {
        int i = ui->tableShortcuts->currentRow();
        if (i >= 0) {
            ui->tableShortcuts->removeRow(i);
            flag_change = true;
        }
    }
}


void ConfShortcuts::slotUp()
{
    if (ui->tableShortcuts->rowCount() <= 1) return;

    int i = ui->tableShortcuts->currentRow();
    QAbstractItemModel* model = ui->tableShortcuts->model();
    if ( (model == NULL) || (i == 0) ) return;

    QString strAmount = model->index(i - 1, 0).data().toString();
    QString strDescr = model->index(i - 1, 1).data().toString();
    QString strPosNr = model->index(i - 1, 2).data().toString();
    model->setData(model->index(i - 1, 0), model->index(i, 0).data().toString());
    model->setData(model->index(i - 1, 1), model->index(i, 1).data().toString());
    model->setData(model->index(i - 1, 2), model->index(i, 2).data().toString());
    model->setData(model->index(i, 0), strAmount);
    model->setData(model->index(i, 1), strDescr);
    model->setData(model->index(i, 2), strPosNr);
    ui->tableShortcuts->setCurrentIndex(model->index(i - 1, 0));
}


void ConfShortcuts::slotDown()
{
    if (ui->tableShortcuts->rowCount() <= 1) return;

    int i = ui->tableShortcuts->currentRow();
    QAbstractItemModel* model = ui->tableShortcuts->model();
    if ( (model == NULL) || (i == ui->tableShortcuts->rowCount() - 1) ) return;

    QString strAmount = model->index(i + 1, 0).data().toString();
    QString strDescr = model->index(i + 1, 1).data().toString();
    QString strPosNr = model->index(i + 1, 2).data().toString();
    model->setData(model->index(i + 1, 0), model->index(i, 0).data().toString());
    model->setData(model->index(i + 1, 1), model->index(i, 1).data().toString());
    model->setData(model->index(i + 1, 2), model->index(i, 1).data().toString());
    model->setData(model->index(i, 0), strAmount);
    model->setData(model->index(i, 1), strDescr);
    model->setData(model->index(i, 2), strPosNr);
    ui->tableShortcuts->setCurrentIndex(model->index(i + 1, 0));
}


void ConfShortcuts::slotModified()
{
    flag_change = true;
}


void ConfShortcuts::slotSelectProfession(int i)
{
    if (i < 0) return;
    if (flag_change) {
        MsgBox box;
        QString msg = QString::fromUtf8("Sie haben Änderungen vorgenommen.\n\nSoll diese gespeichert werden?");
        if (box.execYesNoBox(msg, MsgBox::BTN_YES) == MsgBox::BTN_YES) {
            write();
        }
    }
    loadTable(i);
    curProf = i;
}
