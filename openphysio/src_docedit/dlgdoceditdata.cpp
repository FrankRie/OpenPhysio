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

#include "dlgdoceditdata.h"
#include "ui_dlgdoceditdata.h"
#include "dlgdoceditor.h"


DlgDocEditData::DlgDocEditData(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgDocEditData)
{
    ui->setupUi(this);
    selDataField = "";
    connect(ui->pbCancel, &QPushButton::clicked, this, &DlgDocEditData::reject);
    connect(ui->pbSave, &QPushButton::clicked, this, &DlgDocEditData::slotAccept);
    installEventFilter(this);
}


DlgDocEditData::~DlgDocEditData()
{
    delete ui;
}


bool DlgDocEditData::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        if (ke->key() == Qt::Key_F1 || ke->key() == Qt::Key_Help) {
            //helpWrapper.showHelpForKeyword("", true);
            return true;
        }
    }
    return QDialog::eventFilter(obj, event);
}


void DlgDocEditData::init(cDocTemplate *c)
{
    if (c == NULL) return;
    int dt = c->curTemplate.getTemplateType();
    if (dt < 0 || dt >= cTemplateType::TT_COUNT) return;

    ui->labelData->setText(ui->labelData->text() + '"' + c->getTemplateName(dt) + '"');
    ui->treeWidget->setColumnCount(1);
    QList<QTreeWidgetItem *> items;

    QList<int> list = c->getAllowedCategoryIDs(dt);
    for (int i = 0; i < list.count(); i++) {
        items.append(new QTreeWidgetItem(ui->treeWidget));
        QTreeWidgetItem *item = items.last();
        QString s = c->mapCategories[list.at(i)];
        if (s.isEmpty()) continue;
        item->setText(0, s);
        item->setExpanded(true);
        QStringList l = c->getCategoryItems(list.at(i));
        for (int j = 0; j < l.count(); j++) {
            item->addChild(new QTreeWidgetItem());
            item->child(items.at(items.count() - 1)->childCount() - 1)->setText(0, l.at(j));
        }
    }
    selDataField = "";

    connect(ui->treeWidget, &QTreeWidget::currentItemChanged, this, &DlgDocEditData::slotItemChanged);
    connect(ui->treeWidget, &QTreeWidget::itemActivated, this, &DlgDocEditData::slotItemSelected);
}


QString DlgDocEditData::getSelectedDataField()
{
    return selDataField;
}


void DlgDocEditData::slotItemSelected(QTreeWidgetItem *w, int column)
{
    if (w == NULL) return;

    if (w->childCount() == 0) { // we don't care about root items
        QTreeWidgetItem *p = w->parent();
        if (p != NULL) {
            selDataField = p->text(column) + "." + w->text(column);
            this->accept();
        }
    } else selDataField = "";
}


void DlgDocEditData::slotItemChanged(QTreeWidgetItem *w)
{
    if (w == NULL) return;

    if (w->childCount() == 0) { // we don't care about root items
        QTreeWidgetItem *p = w->parent();
        if (p != NULL) {
            selDataField = p->text(0) + "." + w->text(0);
        }
    } else selDataField = "";
}


void DlgDocEditData::slotAccept()
{
    if (selDataField.isEmpty()) {
        QMessageBox::warning(this, "OpenPhysio", QString::fromUtf8("Es wurde kein Platzhalter ausgewählt"));
    } else {
        accept();
    }
}
