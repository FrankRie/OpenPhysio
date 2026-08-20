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

#include "browsequittances.h"
#include "ui_browsequittances.h"
#include "gendoc.h"
#include "mydb.h"
#include "protect.h"
#include "msgbox.h"


BrowseQuittances::BrowseQuittances(int mode, QWidget *parent) :
    Browse(mode, parent),
    m_ui(new Ui::BrowseQuittances)
{
    m_ui->setupUi(this);
    init((TableViewAnimated*)m_ui->tvQuitt, myDB::TABLE_QUITTANCES, 1, Qt::DescendingOrder);

    // add display filter widget and toolbar
    QGridLayout *layout = (QGridLayout*) this->layout();
    layout->addWidget(&browseToolBar, 0, 0);
    layout->addWidget(&displayFilter, 0, 1);

    actQuittanceView = new QAction(QIcon(":/edit.png"), QString::fromUtf8("Quittung ansehen"), this);
    actQuittanceDel = new QAction(QIcon(":/delete.png"), QString::fromUtf8("Quittung löschen"), this);
    connect(actQuittanceView, &QAction::triggered, this, &BrowseQuittances::slotQuittanceSelected);
    connect(actQuittanceDel, &QAction::triggered, this, &BrowseQuittances::slotQuittanceDel);
    browseToolBar.addAction(actQuittanceView);
    browseToolBar.addAction(actQuittanceDel);
    m_ui->tvQuitt->addAction(actQuittanceView);
    m_ui->tvQuitt->addAction(actQuittanceDel);
    m_ui->tvQuitt->setContextMenuPolicy(Qt::ActionsContextMenu);

    connect(m_ui->tvQuitt, &QTableView::activated, this, &BrowseQuittances::slotQuittanceSelected);
}


BrowseQuittances::~BrowseQuittances()
{
    if (actQuittanceView != NULL) delete actQuittanceView;
    if (actQuittanceDel != NULL) delete actQuittanceDel;
    delete m_ui;
}


void BrowseQuittances::slotQuittanceSelected()
{
    GUARD(Protect::FUNC_QUITT);

    int id = getSelectedItem();
    if (id < 0) {
        MsgBox::info(QString::fromUtf8("Bitte erst eine Quittung wählen!"));
        return;
    }

    cQuittance q;
    db.retrieveItem(q, id);
    if (q.getID() > 0) {
        gendoc.genQuittance(q);
    }
}


void BrowseQuittances::slotQuittanceDel()
{
    GUARD(Protect::FUNC_QUITT_DEL);

    int id = getSelectedItem();
    if (id < 0) {
        MsgBox::info(QString::fromUtf8("Bitte erst eine Quittung wählen!"));
        return;
    }

    MsgBox box;
    if (box.execYesNoBox(QString::fromUtf8("Quittung wirklich löschen?"), MsgBox::BTN_NO) == MsgBox::BTN_YES) {
        if (!db.deleteItem(myDB::TABLE_QUITTANCES, id))
            MsgBox::error(QString::fromUtf8("Fehler beim Löschen der Quittung aufgetreten!"));
    }
}
