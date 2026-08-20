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

#include "browseemployees.h"
#include "mydb.h"
#include "protect.h"
#include "msgbox.h"


BrowseEmployees::BrowseEmployees(int mode, QWidget *parent)
    : Browse(mode, parent)
{
    ui.setupUi(this);
    init((TableViewAnimated*)ui.tvEmpl, myDB::TABLE_EMPLOYEES);

    // add display filter widget and toolbar
    QGridLayout *layout = (QGridLayout*) this->layout();
    layout->addWidget(&browseToolBar, 0, 0);
    layout->addWidget(&displayFilter, 0, 1);

    connect(ui.tvEmpl, &QTableView::activated, this, [this](const QModelIndex &) {
        slotEmplEdit();
    });

    //connect(ui.tvEmpl, SIGNAL(activated(const QModelIndex&)), this, SLOT(slotEmplEdit()));

    actEmplAdd = new QAction(QIcon(":/new_user.png"), QString::fromUtf8("Neuer Mitarbeiter"), this);
    actEmplEdit = new QAction(QIcon(":/edit_user.png"), QString::fromUtf8("Mitarbeiter bearbeiten"), this);
    actEmplDel = new QAction(QIcon(":/delete_user.png"), QString::fromUtf8("Mitarbeiter löschen"), this);
    connect(actEmplAdd, &QAction::triggered, this, &BrowseEmployees::slotEmplAdd);
    connect(actEmplEdit, &QAction::triggered, this, &BrowseEmployees::slotEmplEdit);
    connect(actEmplDel, &QAction::triggered, this, &BrowseEmployees::slotEmplDel);
    browseToolBar.addAction(actEmplAdd);
    browseToolBar.addSeparator();
    browseToolBar.addAction(actEmplEdit);
    browseToolBar.addSeparator();
    browseToolBar.addAction(actEmplDel);
    ui.tvEmpl->addAction(actEmplAdd);
    ui.tvEmpl->addAction(actEmplEdit);
    ui.tvEmpl->addAction(actEmplDel);
    ui.tvEmpl->setContextMenuPolicy(Qt::ActionsContextMenu);

    if (myConf.isTableColorMode()) {
        ui.tvEmpl->setAlternatingRowColors(false);
        QStringList keywords;
        QList<QColor> colors;
        keywords << "inaktiv";
        colors << myConf.getColor("inaktiv");
        setBackgroundKeywords(keywords, colors);
        enableBackgroundColorMode(true, 8);
    }

}


BrowseEmployees::~BrowseEmployees()
{
    if (actEmplAdd != NULL) delete actEmplAdd;
    if (actEmplEdit != NULL) delete actEmplEdit;
    if (actEmplDel != NULL) delete actEmplDel;
}

//
// slotEmplAdd
//
void BrowseEmployees::slotEmplAdd()
{
    GUARD(Protect::FUNC_EMP);
	
    // indicate that the user wants to create a new employee
    DlgEmpl dlgEmpl;
    dlgEmpl.initEmployee(NULL);
    dlgEmpl.exec();
}

//
// slotEmplEdit
//
void BrowseEmployees::slotEmplEdit()
{
    GUARD(Protect::FUNC_EMP);
	
    int id = getSelectedItem();
    if (id < 0) {
        MsgBox::info(QString::fromUtf8("Bitte erst einen Mitarbeiter wählen!"));
        return;
    }

    cEmployee p;
    db.retrieveItem(p, id);
    DlgEmpl dlgEmpl;
    dlgEmpl.initEmployee(&p);
    dlgEmpl.exec();
}

//
// slotEmplDel
//
void BrowseEmployees::slotEmplDel()
{
    GUARD(Protect::FUNC_EMP_DEL);
	
    int id = getSelectedItem();
    if (id < 0) {
        MsgBox::info(QString::fromUtf8("Bitte erst einen Mitarbeiter wählen!"));
        return;
    }

    MsgBox box;
    if (box.execYesNoBox(QString::fromUtf8("Mitarbeiter wirklich löschen?"), MsgBox::BTN_NO) == MsgBox::BTN_YES)
        if (!db.deleteItem(myDB::TABLE_EMPLOYEES, id))
            MsgBox::error(QString::fromUtf8("Fehler beim Löschen des Mitarbeiters aufgetreten!"));
}

