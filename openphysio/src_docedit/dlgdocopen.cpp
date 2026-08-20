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

#include "dlgdocopen.h"
#include "ui_dlgdocopen.h"
#include "msgbox.h"


DlgDocOpen::DlgDocOpen(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgDocOpen)
{
    ui->setupUi(this);

    init(false);

    connect(ui->pbCancel, &QPushButton::clicked, this, &DlgDocOpen::reject);
    connect(ui->pbOpen, &QPushButton::clicked, this, &DlgDocOpen::slotOpen);
    connect(ui->pbDel, &QPushButton::clicked, this, &DlgDocOpen::slotDel);
    connect(ui->treeWidget, &QTreeWidget::currentItemChanged, this, &DlgDocOpen::slotItemChanged);
    connect(ui->treeWidget, &QTreeWidget::itemActivated, this, &DlgDocOpen::slotItemSelected);

}


DlgDocOpen::~DlgDocOpen()
{
    delete ui;
    while (!listTemplateTypes.isEmpty()) delete listTemplateTypes.takeFirst();
    while (!listTemplates.isEmpty()) delete listTemplates.takeFirst();
}


void DlgDocOpen::init(bool re_init)
{
    ui->treeWidget->clear();

    selTemplate = "";
    if (!re_init) {
        db.retrieveItemList(myDB::TABLE_TEMPLATE_TYPES, reinterpret_cast<QList<cTableItem*>&>(listTemplateTypes));
    }
    while (!listTemplates.isEmpty()) delete listTemplates.takeFirst();
    db.retrieveItemList(myDB::TABLE_TEMPLATES, reinterpret_cast<QList<cTableItem*>&>(listTemplates));
    // decompress template data
    for (int i = 0; i < listTemplates.count(); i++) {
        QByteArray qba = QByteArray::fromBase64(listTemplates.at(i)->getContents());
        qba = qUncompress(qba);
        listTemplates.at(i)->setContents(qba);
    }

    QList<QTreeWidgetItem *> items;

    for (int i = 0; i < listTemplateTypes.count(); i++) {
        items.append(new QTreeWidgetItem(ui->treeWidget));
        QTreeWidgetItem *item = items.last();
        item->setText(0, listTemplateTypes.at(i)->getName());
        item->setExpanded(true);
        for (int j = 0; j < listTemplates.count(); j++) {
            if (listTemplates.at(j)->getTemplateType() == listTemplateTypes.at(i)->getID()) {
                item->addChild(new QTreeWidgetItem());
                item->child(items.at(items.count() - 1)->childCount() - 1)->setText(0, listTemplates.at(j)->getName());
            }
        }
    }
}


cTemplate DlgDocOpen::getSelectedTemplate()
{
    cTemplate tmpl;
    QTreeWidgetItem *w = ui->treeWidget->currentItem();
    if (!w) return tmpl;
    if (!w->parent()) return tmpl; // do not try to delete root items

    for (int i = 0; i < listTemplates.count(); i++) {
        if (listTemplates.at(i)->getName() == w->text(0)) {
            tmpl = *listTemplates.at(i);
            break;
        }
    }
    return tmpl;
}


void DlgDocOpen::slotItemSelected(QTreeWidgetItem *w, int column)
{
    selTemplate = "";
    if (w == NULL) return;
    if (!w->parent()) return;

    selTemplate = w->text(column);
    this->accept();
}


void DlgDocOpen::slotItemChanged(QTreeWidgetItem *w)
{
    selTemplate = "";
    if (w == NULL) return;
    if (!w->parent()) return;
    selTemplate = w->text(0);
}


void DlgDocOpen::slotOpen()
{
    if (selTemplate.isEmpty()) {
        QMessageBox::warning(this, "OpenPhysio", QString::fromUtf8("Es wurde keine Vorlage ausgewählt"));
    } else {
        accept();
    }
}


void DlgDocOpen::slotDel()
{
    cTemplate ct = getSelectedTemplate();
    if (ct.getID() < 0) return;

    MsgBox box;
    if (box.execYesNoBox(QString::fromUtf8("Diese Vorlage wirklich löschen?"), MsgBox::BTN_NO) == MsgBox::BTN_NO) return;

    if (!db.deleteItem(myDB::TABLE_TEMPLATES, ct.getID())) {
        QMessageBox::warning(this, "OpenPhysio", QString::fromUtf8("Die Vorlage konnte nicht gelöscht werden"));
    } else {
        init(true);
    }
}
