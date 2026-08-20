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

#include "browsephysicians.h"
#include "mydb.h"
#include "protect.h"
#include "msgbox.h"


BrowsePhysicians::BrowsePhysicians(int mode, QWidget *parent)
    : Browse(mode, parent)
{
    ui.setupUi(this);
    init((TableViewAnimated*)ui.tvPhys, myDB::TABLE_PHYSICIANS);

    // add display filter widget and toolbar
    QGridLayout *layout = (QGridLayout*) this->layout();
    layout->addWidget(&browseToolBar, 0, 0);
    layout->addWidget(&displayFilter, 0, 1);

    if (mode == MODE_SELECT) {
	connect(ui.tvPhys, SIGNAL(activated(const QModelIndex&)), this, SLOT(slotPhysSelected()));
    } else {
	connect(ui.tvPhys, SIGNAL(activated(const QModelIndex&)), this, SLOT(slotPhysEdit()));
    }
	
    actPhysAdd = new QAction(QIcon(":/physician_add.png"), QString::fromUtf8("Neuer Arzt"), this);
    actPhysEdit = new QAction(QIcon(":/physician.png"), QString::fromUtf8("Arzt bearbeiten"), this);
    actPhysDel = new QAction(QIcon(":/physician_del.png"), QString::fromUtf8("Arzt löschen"), this);
    connect(actPhysAdd, SIGNAL(triggered()), this, SLOT(slotPhysAdd()));
    connect(actPhysEdit, SIGNAL(triggered()), this, SLOT(slotPhysEdit()));
    connect(actPhysDel, SIGNAL(triggered()), this, SLOT(slotPhysDel()));
    browseToolBar.addAction(actPhysAdd);
    browseToolBar.addSeparator();
    browseToolBar.addAction(actPhysEdit);
    browseToolBar.addSeparator();
    browseToolBar.addAction(actPhysDel);
    ui.tvPhys->addAction(actPhysAdd);
    ui.tvPhys->addAction(actPhysEdit);
    ui.tvPhys->addAction(actPhysDel);
    ui.tvPhys->setContextMenuPolicy(Qt::ActionsContextMenu);
}


BrowsePhysicians::~BrowsePhysicians()
{
    if (actPhysAdd != NULL) delete actPhysAdd;
    if (actPhysEdit != NULL) delete actPhysEdit;
    if (actPhysDel != NULL) delete actPhysDel;
}


void BrowsePhysicians::showDlg(cPhys *p)
{
    DlgPhys dlgPhys;
    dlgPhys.initPhys(p);
    dlgPhys.exec();
}


cPhys& BrowsePhysicians::getSelPhysician()
{
    return selPhys;
}


void BrowsePhysicians::slotPhysSelected()
{
    int id = getSelectedItem();
    if (id < 0) return;

    db.retrieveItem(selPhys, id);
    emit sigItemSelected();
}


void BrowsePhysicians::slotPhysAdd()
{
    GUARD(Protect::FUNC_PHY);
	
    // indicate that the user wants to create a new physician
    DlgPhys dlgPhys;
    cPhys p;
    dlgPhys.initPhys(&p);
    dlgPhys.exec();
}


void BrowsePhysicians::slotPhysEdit()
{
    GUARD(Protect::FUNC_PHY);

    int id = getSelectedItem();
    if (id < 0) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Bitte erst einen Arzt wählen!\n"), QMessageBox::Ok);
        return;
    }

    DlgPhys dlgPhys;
    cPhys p;
    db.retrieveItem(p, id);
    dlgPhys.initPhys(&p);
    dlgPhys.exec();
}


void BrowsePhysicians::slotPhysDel()
{
    GUARD(Protect::FUNC_PHY_DEL);

    int id = getSelectedItem();
    if (id < 0) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Bitte erst einen Arzt wählen!\n"), QMessageBox::Ok);
        return;
    }

    MsgBox box;
    if (box.execYesNoBox(QString::fromUtf8("Arzt wirklich löschen?"), MsgBox::BTN_NO) == MsgBox::BTN_YES) {
	db.deleteItem(myDB::TABLE_PHYSICIANS, id);
    }
}
