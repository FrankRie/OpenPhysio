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

#include "browsepatients.h"
#include "protect.h"
#include "msgbox.h"
#include "browsefindings.h"


BrowsePatients::BrowsePatients(int mode, QWidget *parent)
    : Browse(mode, parent)
{
    ui.setupUi(this);

    tvPatient = new TableViewAnimated(this);
    tvPatient->setObjectName(QString::fromUtf8("tvPatient"));
    tvPatient->setFrameShape(QFrame::StyledPanel);
    tvPatient->setFrameShadow(QFrame::Sunken);
    tvPatient->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    tvPatient->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    tvPatient->setAlternatingRowColors(false);
    tvPatient->setSelectionMode(QAbstractItemView::SingleSelection);
    tvPatient->setSelectionBehavior(QAbstractItemView::SelectRows);

    tvPatient->setStyleSheet(
        // 1. Verhindert die unschönen Fokus-Balken/Rahmen unter Windows
        "QTableView { outline: 0; }"

        // 2. Ändert NUR die Elemente (:item), die aktiv ausgewählt (:selected) sind
        "QTableView::item:selected {"
        "   background-color: #0078d7 !important;" // !important zwingt Windows zur Übernahme
        "   color: #ffffff !important;"            // Weißer Text für gute Lesbarkeit
        "}"

        // 3. Stellt sicher, dass nicht-ausgewählte Zellen ihren Standard-Hintergrund behalten
        "QTableView::item {"
        "   background-color: transparent;"
        "}"
    );

    tvPatient->setSortingEnabled(true);


    actPatientAdd = actPatientEdit = actPatientDel = actViewReceipes = NULL;
    actViewFindings = actViewReports = NULL;
    actAcceptSelection = NULL;

    ui.gridLayout->addWidget(tvPatient, 1, 0, 1, 4);

    init(tvPatient, myDB::TABLE_PATIENTS);

    // add display filter widget and toolbar
    QGridLayout *layout = (QGridLayout*) this->layout();
    layout->addWidget(&browseToolBar, 0, 0);
    layout->addWidget(&displayFilter, 0, 1);

    cbFilterActive = new QCheckBox(this);
    layout->addWidget(cbFilterActive, 0, 2);
    cbFilterActive->setChecked(true);
    cbFilterActive->setText("Zeige nur aktive Patienten");
    connect(cbFilterActive, &QCheckBox::toggled, this, &BrowsePatients::slotFilterPatients);

    TableFilter tf(myDB::TABLE_PATIENTS);
    tf.clear();
    tf.addFilterExpr(true, TableFilter::F_PAT_STATE, TableFilter::OP_EQUAL, cPatient::STATE_ACTIVE);
    displayFilter.clearFilterField();
    setFilter(&tf);


    listSelPatients.clear();

    if (mode == MODE_FULL) {
        connect(tvPatient, &QTableView::activated, this, &BrowsePatients::slotPatientEdit);
    } else if (mode == MODE_SELECT) {
        connect(tvPatient, &QTableView::activated, this, &BrowsePatients::slotPatientSelected);
    } else if (mode == MODE_MULTISELECT) {
        tvPatient->setSelectionBehavior(QAbstractItemView::SelectRows);
        tvPatient->setSelectionMode(QAbstractItemView::MultiSelection);
        connect(tvPatient, &QTableView::activated, this, &BrowsePatients::slotSelectMulti);
        actAcceptSelection = new QAction(QIcon(":/accept.png"), QString::fromUtf8("Auswahl übernehmen"), this);
        connect(actAcceptSelection, &QAction::triggered, this, &BrowsePatients::slotSelectMulti);
        browseToolBar.addAction(actAcceptSelection);
        tvPatient->addAction(actAcceptSelection);
    } else if (mode == MODE_CASH) {
        connect(tvPatient, &QTableView::activated, this, &BrowsePatients::slotPatientSelected);
    }

    if (mode < MODE_MULTISELECT) {
        actPatientAdd = new QAction(QIcon(":/new_user.png"), QString::fromUtf8("Neuer Patient"), this);
        actPatientEdit = new QAction(QIcon(":/edit_user.png"), QString::fromUtf8("Patient bearbeiten"), this);
        actPatientDel = new QAction(QIcon(":/delete_user.png"), QString::fromUtf8("Patient löschen"), this);
        actViewReceipes = new QAction(QIcon(":/viewreceipe.png"), QString::fromUtf8("Rezepte einsehen"), this);
        actViewFindings = new QAction(QIcon(":/finding.png"), QString::fromUtf8("Befunde einsehen"), this);
        actViewReports = new QAction(QIcon(":/report.png"), QString::fromUtf8("Therapieberichte einsehen"), this);
        connect(actPatientAdd, &QAction::triggered, this, &BrowsePatients::slotPatientAdd);
        connect(actPatientEdit, &QAction::triggered, this, &BrowsePatients::slotPatientEdit);
        connect(actPatientDel, &QAction::triggered, this, &BrowsePatients::slotPatientDel);
        connect(actViewReceipes, &QAction::triggered, this, &BrowsePatients::slotViewReceipes);
        connect(actViewFindings, &QAction::triggered, this, &BrowsePatients::slotViewFindings);
        connect(actViewReports, &QAction::triggered, this, &BrowsePatients::slotViewReports);
        browseToolBar.addAction(actPatientAdd);
        browseToolBar.addSeparator();
        browseToolBar.addAction(actPatientEdit);
        browseToolBar.addSeparator();
        browseToolBar.addAction(actPatientDel);
        browseToolBar.addSeparator();
        browseToolBar.addAction(actViewReceipes);
        browseToolBar.addSeparator();
        browseToolBar.addAction(actViewFindings);
        browseToolBar.addSeparator();
        browseToolBar.addAction(actViewReports);
        tvPatient->addAction(actPatientAdd);
        tvPatient->addAction(actPatientEdit);
        tvPatient->addAction(actPatientDel);
        tvPatient->addAction(actViewReceipes);
        tvPatient->addAction(actViewFindings);
        tvPatient->addAction(actViewReports);
    }
    tvPatient->setContextMenuPolicy(Qt::ActionsContextMenu);

    kread = new KvkRead();
}


BrowsePatients::~BrowsePatients()
{
    if (kread != NULL) delete kread;
    if (actPatientAdd != NULL) delete actPatientAdd;
    if (actPatientEdit != NULL) delete actPatientEdit;
    if (actPatientDel != NULL) delete actPatientDel;
    if (actViewReceipes != NULL) delete actViewReceipes;
    if (actViewFindings != NULL) delete actViewFindings;
    if (actViewReports != NULL) delete actViewReports;
    if (actAcceptSelection != NULL) delete actAcceptSelection;
    delete tvPatient;
}


void BrowsePatients::showDlg(cPatient *p)
{
    DlgPatient dlgPatient;
    dlgPatient.initPatient(p);
    dlgPatient.exec();
}


cPatient& BrowsePatients::getSelPatient()
{
    return selPatient;
}


void BrowsePatients::slotPatientSelected()
{
    int id = getSelectedItem();
    if (id < 0) return;

    if (curMode == MODE_CASH) {
        emit sigPatientSelected(id);
    } else {
        db.retrieveItem(selPatient, id);
        emit sigItemSelected();
    }
}

//
// slotPatientAdd
//
void BrowsePatients::slotPatientAdd()
{
    GUARD(Protect::FUNC_PAT);
	
    DlgPatient dlgPatient;
    // indicate that the user wants to create a new patient
    cPatient p;
    dlgPatient.initPatient(&p);
    dlgPatient.exec();
}

//
// slotPatientEdit
//
void BrowsePatients::slotPatientEdit()
{
    GUARD(Protect::FUNC_PAT);

    int id = getSelectedItem();
    if (id < 0) {
        MsgBox::info(QString::fromUtf8("Bitte erst einen Patienten wählen!"));
        return;
    }
	
    cPatient p;
    if (db.retrieveItem(p, id) == true) {
	DlgPatient dlgPatient;
        dlgPatient.initPatient(&p);
	dlgPatient.exec();
    }
}

//
// slotPatientDel
//
void BrowsePatients::slotPatientDel()
{
    GUARD(Protect::FUNC_PAT_DEL);

    int id = getSelectedItem();
    if (id < 0) {
        MsgBox::info(QString::fromUtf8("Bitte erst einen Patienten wählen!"));
        return;
    }

    MsgBox box;
    if (box.execYesNoBox(QString::fromUtf8("Patient wirklich löschen?\n\nACHTUNG: Es werden auch die zugehörigen Behandlungstermine, Befunde und Rezepte dieses Patienten gelöscht!!!"), MsgBox::BTN_NO) == MsgBox::BTN_YES) {
        if (!db.deleteItem(myDB::TABLE_PATIENTS, id))
            MsgBox::error(QString::fromUtf8("Fehler beim Löschen des Patienten aufgetreten!"));
    }
}


void BrowsePatients::slotReadChipcard()
{
    GUARD(Protect::FUNC_READCHIP);
	
    QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Bitte Chipkarte einlegen!\n"), QMessageBox::Ok);
	
    DlgPatient dlgPatient;
    if (dlgPatient.readChip()) dlgPatient.exec();
    else QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Chipkarte konnte nicht gelesen werden!\n"), QMessageBox::Ok);
}


void BrowsePatients::showPatientData(int i)
{
    int id = getSelectedItem();
    if (id < 0) {
        MsgBox::info(QString::fromUtf8("Bitte erst einen Patienten wählen!"));
	return;
    }

    Browse *w = NULL;
    QString str;
    cPatient p;
    //int filter_field = -1;

    if (!db.retrieveItem(p, id)) return;

    switch(i) {
	case DATA_RECEIPES: {
	    str = "Rezepte";
	    BrowseReceipes *browse = new BrowseReceipes();
            browse->setCurPatient(p);
            //filter_field = Browse::F_REC_PID;
	    w = browse; }
	    break;
	case DATA_REPORTS: {
	    str = "Therapieberichte";
	    BrowseReports *browse = new BrowseReports();
	    w = browse; }
	    break;
	case DATA_FINDINGS: {
	    str = "Befunde";
	    BrowseFindings *browse = new BrowseFindings();
	    browse->setPatientID(id);
	    w = browse; }
	    break;
	default: break;
    }
    if (w == NULL) return;

    QDialog qd;
    qd.setWindowIcon(QIcon(":/favicon.png"));
    qd.setWindowTitle(str + QString::fromUtf8(" für ") + p.getForename() + " " + p.getName());
    qd.resize(700, 400);

    TableFilter tf(w->getTableType());
    tf.addFilterExpr(true, TableFilter::F_PAT_ID, TableFilter::OP_EQUAL, p.getID());
    w->setFilter(&tf);

    //w->setFilter(filter_field, p.getID(), Browse::OP_EQUAL);

    QGridLayout gridLayout(&qd);
    gridLayout.addWidget(w, 0, 0, 1, 1);
    qd.exec();

    if (w != NULL) delete w;
}


void BrowsePatients::slotViewReceipes()
{
    showPatientData(DATA_RECEIPES);
}


void BrowsePatients::slotViewFindings()
{
    showPatientData(DATA_FINDINGS);
}


void BrowsePatients::slotViewReports()
{
    showPatientData(DATA_REPORTS);
}


void BrowsePatients::slotSelectMulti()
{
    QItemSelectionModel *model = tvPatient->selectionModel();
    QModelIndexList indexes = model->selectedRows();
	
    if (indexes.count() <= 0) return;
    for (int i = 0; i < indexes.count(); i++) {
        // put row (not patient's ID) into list
        // patient's ID would be: indexes.at(i).data().toInt()
        //listSelPatients.append(indexes.at(i).row());
        listSelPatients.append(indexes.at(i).data().toInt());
    }
    emit sigItemSelected();
}


QList<int> BrowsePatients::getSelectedPatients()
{
    return listSelPatients;
}


void BrowsePatients::slotFilterPatients(bool b)
{
    TableFilter tf(myDB::TABLE_PATIENTS);
    tf.clear();
    if (b) {
        tf.addFilterExpr(true, TableFilter::F_PAT_STATE, TableFilter::OP_EQUAL, cPatient::STATE_ACTIVE);
    }
    displayFilter.clearFilterField();
    setFilter(&tf);
}
