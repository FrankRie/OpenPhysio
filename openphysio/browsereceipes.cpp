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

#include "browsereceipes.h"
#include "dlgreceipe.h"
#include "mydb.h"
#include "protect.h"
#include "msgbox.h"
#include "dlgreceipepriv.h"
#include "dlgreceipeprof.h"


BrowseReceipes::BrowseReceipes(int mode, QWidget *parent)
    : Browse(mode, parent)
{
    ui.setupUi(this);

    tvReceipes = new TableViewAnimated(this);
    tvReceipes->setObjectName(QString::fromUtf8("tvReceipes"));
    tvReceipes->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    tvReceipes->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    tvReceipes->setAlternatingRowColors(true);
    tvReceipes->setSelectionMode(QAbstractItemView::SingleSelection);
    tvReceipes->setSelectionBehavior(QAbstractItemView::SelectRows);

    tvReceipes->setStyleSheet(
        // 1. Verhindert die unschönen Fokus-Balken/Rahmen unter Windows
        "QTableView { outline: 0; }"

        // 2. Ändert NUR die Elemente (:item), die aktiv ausgewählt (:selected) sind
        "QTableView::item:selected {"
        "   background-color: #0078d7 !important;" // !important zwingt Windows zur Übernahme
        "   color: #ffffff !important;"
        "}"
    );

    tvReceipes->setSortingEnabled(true);
    ui.gridLayout->addWidget(tvReceipes, 1, 0, 1, 4);

    switch(mode) {
        case MODE_CASH_ADDPAY:
            init(tvReceipes, myDB::TABLE_CASH_ADDPAY, 1, Qt::DescendingOrder, 2);
            break;
        case MODE_CASH_SELFPAY:
            init(tvReceipes, myDB::TABLE_CASH_SELFPAY, 1, Qt::DescendingOrder, 2);
            break;
        case MODE_CASH_PRIVRECEIPE:
            init(tvReceipes, myDB::TABLE_CASH_PRIVRECEIPE, 1, Qt::DescendingOrder, 2);
            break;
        default:
            init(tvReceipes, myDB::TABLE_RECEIPES, 1, Qt::DescendingOrder, 2);
            break;
    }

    // add display filter widget and toolbar
    QGridLayout *layout = (QGridLayout*) this->layout();
    layout->addWidget(&browseToolBar, 0, 0);
    layout->addWidget(&displayFilter, 0, 1);

    actReceipeAdd = actReceipePrivateAdd = actReceipeCopy = NULL;
    actReceipeEdit = actReceipeDel = NULL;

    if (mode == MODE_FULL || mode == MODE_SELECT) {
        actReceipeAdd = new QAction(QIcon(":/new.png"), QString::fromUtf8("Neues Kassenrezept"), this);
        actReceipePrivateAdd = new QAction(QIcon(":/new_private.png"), QString::fromUtf8("Neues Privatrezept"), this);
        actReceipeCopy = new QAction(QIcon(":/editcopy.png"), QString::fromUtf8("Neues Rezept als Kopie"), this);
        actReceipeEdit = new QAction(QIcon(":/edit.png"), QString::fromUtf8("Rezept bearbeiten"), this);
        actReceipeDel = new QAction(QIcon(":/delete.png"), QString::fromUtf8("Rezept löschen"), this);
        connect(actReceipeAdd, SIGNAL(triggered()), this, SLOT(slotReceipeAdd()));
        connect(actReceipePrivateAdd, SIGNAL(triggered()), this, SLOT(slotReceipePrivateAdd()));
        connect(actReceipeEdit, SIGNAL(triggered()), this, SLOT(slotReceipeEdit()));
        connect(actReceipeDel, SIGNAL(triggered()), this, SLOT(slotReceipeDel()));
        connect(actReceipeCopy, SIGNAL(triggered()), this, SLOT(slotReceipeCopy()));
        browseToolBar.addAction(actReceipeAdd);
        browseToolBar.addSeparator();
        browseToolBar.addAction(actReceipePrivateAdd);
        browseToolBar.addSeparator();
        browseToolBar.addAction(actReceipeEdit);
        browseToolBar.addSeparator();
        browseToolBar.addAction(actReceipeCopy);
        browseToolBar.addSeparator();
        browseToolBar.addAction(actReceipeDel);
        tvReceipes->addAction(actReceipeAdd);
        tvReceipes->addAction(actReceipePrivateAdd);
        tvReceipes->addAction(actReceipeEdit);
        tvReceipes->addAction(actReceipeCopy);
        tvReceipes->addAction(actReceipeDel);
        if (mode == MODE_SELECT) connect(tvReceipes, SIGNAL(activated(const QModelIndex&)), this, SLOT(slotReceipeSelected()));
        else connect(tvReceipes, SIGNAL(activated(const QModelIndex&)), this, SLOT(slotReceipeEdit()));
    }
    else if (mode == MODE_BILL) {
        actReceipeEdit = new QAction(QString::fromUtf8("Rezept bearbeiten"), this);
        connect(actReceipeEdit, SIGNAL(triggered()), this, SLOT(slotReceipeEdit()));
        tvReceipes->addAction(actReceipeEdit);
        connect(tvReceipes, SIGNAL(activated(const QModelIndex&)), this, SLOT(slotReceipeEdit()));
    }
    else if (mode == MODE_EMBEDDED) {
        actReceipeAdd = new QAction(QString::fromUtf8("Neues Kassenrezept"), this);
        actReceipePrivateAdd = new QAction(QString::fromUtf8("Neues Privatrezept"), this);
        actReceipeCopy = new QAction(QString::fromUtf8("Neues Rezept als Kopie"), this);
        actReceipeEdit = new QAction(QString::fromUtf8("Rezept bearbeiten"), this);
        actReceipeDel = new QAction(QString::fromUtf8("Rezept löschen"), this);
        connect(actReceipeAdd, SIGNAL(triggered()), this, SLOT(slotReceipeAdd()));
        connect(actReceipePrivateAdd, SIGNAL(triggered()), this, SLOT(slotReceipePrivateAdd()));
        connect(actReceipeEdit, SIGNAL(triggered()), this, SLOT(slotReceipeEdit()));
        connect(actReceipeDel, SIGNAL(triggered()), this, SLOT(slotReceipeDel()));
        connect(actReceipeCopy, SIGNAL(triggered()), this, SLOT(slotReceipeCopy()));
        tvReceipes->addAction(actReceipeAdd);
        tvReceipes->addAction(actReceipePrivateAdd);
        tvReceipes->addAction(actReceipeCopy);
        tvReceipes->addAction(actReceipeEdit);
        tvReceipes->addAction(actReceipeDel);
        connect(tvReceipes, SIGNAL(activated(const QModelIndex&)), this, SLOT(slotReceipeEdit()));
    } else {
        // MODE_CASH_*
        connect(tvReceipes, SIGNAL(clicked(const QModelIndex&)), this, SLOT(slotAmountChanged(const QModelIndex&)));

    }

    tvReceipes->setContextMenuPolicy(Qt::ActionsContextMenu);

    if (myConf.isTableColorMode() && (mode < MODE_CASH_ADDPAY) ) {
        tvReceipes->setAlternatingRowColors(false);
	QStringList keywords;
	QList<QColor> colors;
	keywords << "Abgeschlossen" << "Abgebrochen" << "Abgerechnet";
    // colors << QColor(189, 194, 231, 255) << QColor(189, 194, 231, 255) << QColor(129, 220, 157, 255);
	colors << myConf.getColor("Abgeschlossen") << myConf.getColor("Abgebrochen") << myConf.getColor("Abgerechnet");
	setBackgroundKeywords(keywords, colors);
	enableBackgroundColorMode(true, 6);
    }
}


BrowseReceipes::~BrowseReceipes()
{
    if (actReceipeAdd != NULL) delete actReceipeAdd;
    if (actReceipePrivateAdd != NULL) delete actReceipePrivateAdd;
    if (actReceipeCopy != NULL) delete actReceipeCopy;
    if (actReceipeEdit != NULL) delete actReceipeEdit;
    if (actReceipeDel != NULL) delete actReceipeDel;
    delete tvReceipes;
}


void BrowseReceipes::setCurPatient(cPatient& p)
{
    // we will use this data to pre-initialize a receipe dialog with a certain patient
    // (cf. method "showDlg")
    curPatient = p;
}


void BrowseReceipes::showDlg(int t)
{
    GUARD(Protect::FUNC_REC);
	
    if (t == RECEIPE_TYPE_GKV) {
        int prof = -1;
        int prof_tmp = db.getSurgery().getProfession();
        // FIXME: remove hard-coded values
        if (prof_tmp == 1) prof = cProfession::PROF_PHYSIOTHERAPY;
        else if (prof_tmp == 2) prof = cProfession::PROF_ERGOTHERAPY;
        else if (prof_tmp == 4) prof = cProfession::PROF_LOGOPEDICS;
        else {
            DlgReceipeProf dlg;
            prof  = dlg.exec();
        }
        if (prof < 0) return;
        DlgReceipe dlgReceipe;
        if (curPatient.getID() < 0) dlgReceipe.initDlg(NULL, prof);
        else dlgReceipe.initDlg(NULL, prof, &curPatient);
        dlgReceipe.exec();
    } else {
        DlgReceipePriv dlgReceipe;
        if (curPatient.getID() < 0) dlgReceipe.initDlg(NULL);
        else dlgReceipe.initDlg(NULL, &curPatient);
        dlgReceipe.exec();
    }
}


void BrowseReceipes::slotReceipeAdd()
{
    showDlg(RECEIPE_TYPE_GKV);
}


void BrowseReceipes::slotReceipePrivateAdd() 
{
    showDlg(RECEIPE_TYPE_PKV);
}


void BrowseReceipes::slotReceipeCopy()
{
    GUARD(Protect::FUNC_REC);

    int id = getSelectedItem();
    if (id < 0) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Bitte erst ein Rezept wählen, das Sie kopieren möchten!\n"), QMessageBox::Ok);
        return;
    }

    if (id >= 0) {
        cReceipe r;
	db.retrieveItem(r, id);
        r.setID(-1);
	if (r.getType() < 3) {
            DlgReceipe dlgReceipe;
            dlgReceipe.initDlg(&r, r.getProfession());
            dlgReceipe.exec();
        } else {
            DlgReceipePriv dlgReceipe;
            dlgReceipe.initDlg(&r);
            dlgReceipe.exec();
        }
    }
}


void BrowseReceipes::slotReceipeEdit()
{
    GUARD(Protect::FUNC_REC);
	
    int id = getSelectedItem();
    if (id < 0) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Bitte erst ein Rezept wählen!\n"), QMessageBox::Ok);
        return;
    }
	
    if (id >= 0) {
        cReceipe r;
	db.retrieveItem(r, id);
	
	if (r.getType() < 3) {
            DlgReceipe dlgReceipe;
            dlgReceipe.initDlg(&r, r.getProfession());
            dlgReceipe.exec();
        } else {
            DlgReceipePriv dlgReceipe;
            dlgReceipe.initDlg(&r);
            dlgReceipe.exec();
        }
    }
}


void BrowseReceipes::slotReceipeDel()
{
    GUARD(Protect::FUNC_REC_DEL);

    int id = getSelectedItem();
    if (id < 0) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Bitte erst ein Rezept wählen!\n"), QMessageBox::Ok);
        return;
    }
	
    MsgBox box;
    if (box.execYesNoBox(QString::fromUtf8("Rezept wirklich löschen?\n(Es werden auch die Behandlungstermine gelöscht!)"), MsgBox::BTN_NO) == MsgBox::BTN_YES) {
	if (db.deleteItem(myDB::TABLE_RECEIPES, id) == false) {
            QMessageBox::critical(0, QObject::tr("Datenbankfehler"), QString::fromUtf8("Rezept konnte nicht gelöscht werden."));
	}
    }
}


void BrowseReceipes::slotReceipeSelected()
{
    int id = getSelectedItem();
    if (id < 0) return;

    db.retrieveItem(selReceipe, id);
    emit sigItemSelected();
}


cReceipe& BrowseReceipes::getSelReceipe()
{
    return selReceipe;
}


void BrowseReceipes::slotAmountChanged(const QModelIndex& index)
{
    if (!index.isValid()) return;
    int id = getSelectedItem();
    if (id < 0) return;

    QAbstractItemModel* m = tvReceipes->model();
    if (m == NULL) return;

    // read the addpay amount from the table view (column: 5)
    // and signal the value to BrowseCash
    QString amount;
    if (curMode == MODE_CASH_ADDPAY) amount = m->index(index.row(), 5).data(0).toString();
    if (curMode == MODE_CASH_SELFPAY) amount = m->index(index.row(), 4).data(0).toString();
    if (curMode == MODE_CASH_PRIVRECEIPE) amount = m->index(index.row(), 4).data(0).toString();
    emit sigAmountChanged(amount, id);
}
