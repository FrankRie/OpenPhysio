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

#include "browseinvoices.h"
#include "config.h"
#include "mydb.h"
#include "gendoc.h"
#include "protect.h"
#include "gendoc.h"
#include "msgbox.h"

#define PAYSTATE_REMINDER  1


BrowseInvoices::BrowseInvoices(int mode, QWidget *parent)
    : Browse(mode, parent)
{
    ui.setupUi(this);
    init((TableViewAnimated*)ui.tvInvoices, myDB::TABLE_INVOICES, 1, Qt::DescendingOrder);

    ui.tvInvoices->setStyle(new NoFocusStyle());

    // =========================================================================
    // HIER DAS STYLESHEET FÜR DIE RECHNUNGEN EINORDNEN:
    // =========================================================================
    ui.tvInvoices->setStyleSheet(
        // Schaltet die Standard-Outlines ab
        "QTableView { outline: none; }"

        // Zwingt Windows, bei JEDEM fokussierten Element (egal ob ausgewählt oder nicht)
        // jegliche Rahmenfarbe und Hintergrund-Überlagerung auf "transparent" zu setzen
        "QTableView::item:focus, QTableView::item:selected:focus, QTableView::item:active:focus {"
        "   outline: none !important;"
        "   border: 0px solid transparent !important;"
        "   border-style: none !important;"
        "}"

        // Das dezente Auswahl-Blau für die Zeile bleibt bestehen
        "QTableView::item:selected {"
        "   background-color: rgba(0, 120, 215, 0.4) !important;"
        "   color: #000000 !important;"
        "}"
    );


    // add display filter widget and toolbar
    QGridLayout *layout = (QGridLayout*) this->layout();
    layout->addWidget(&browseToolBar, 0, 0);
    layout->addWidget(&displayFilter, 0, 1);

    connect(ui.tvInvoices, &QTableView::activated, this, [this](const QModelIndex &) {
        slotInvoiceSelected();
    });

    //connect(ui.tvInvoices, SIGNAL(activated(const QModelIndex&)), this, SLOT(slotInvoiceSelected()));

    actInvoiceView = new QAction(QIcon(":/edit.png"), QString::fromUtf8("Rechnung ansehen"), this);
    //actInvoiceAdd = new QAction(QString::fromUtf8("Neue Rechnung"), this);
    actInvoiceDel = new QAction(QIcon(":/delete.png"), QString::fromUtf8("Rechnung löschen"), this);
    actReminder = new QAction(QIcon(":/reminder.png"), QString::fromUtf8("Mahnung erstellen"), this);

    connect(actInvoiceView, &QAction::triggered, this, &BrowseInvoices::slotInvoiceSelected);
    //connect(actInvoiceAdd, SIGNAL(triggered()), this, &BrowseInvoices::slotInvoiceAdd0);
    connect(actInvoiceDel, &QAction::triggered, this, &BrowseInvoices::slotInvoiceDel);
    connect(actReminder, &QAction::triggered, this, &BrowseInvoices::slotReminder);

    browseToolBar.addAction(actInvoiceView);
    browseToolBar.addSeparator();
    browseToolBar.addAction(actReminder);
    browseToolBar.addAction(actInvoiceDel);

    customMenu = new QMenu("Status..");
    for (int i = 0; i < db.listInvoiceStates.count(); i++) {
        customMenu->addAction(db.listInvoiceStates.at(i)->getName(), this, SLOT(slotStatus()));
        customMenu->actions().last()->setData(i);
    }

    btnStatus = new QToolButton();
    btnStatus->setText("");
    btnStatus->setToolTip(QString::fromUtf8("Status ändern"));
    btnStatus->setIcon(QIcon(":/invstat.png"));
    btnStatus->setMenu(customMenu);
    btnStatus->setPopupMode(QToolButton::InstantPopup);
    browseToolBar.addWidget(btnStatus);

    ui.tvInvoices->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui.tvInvoices, &QWidget::customContextMenuRequested, this, &BrowseInvoices::slotContextMenu);

    if (myConf.isTableColorMode()) {
	ui.tvInvoices->setAlternatingRowColors(false);
	QStringList keywords;
	QList<QColor> colors;
	keywords << "Offen" << "Angemahnt";
	//colors << QColor(231, 189, 45, 255) << QColor(231, 98, 45, 255);
	colors << myConf.getColor("Offen") << myConf.getColor("Angemahnt");
	setBackgroundKeywords(keywords, colors);
	enableBackgroundColorMode(true, 6);
    }
}


BrowseInvoices::~BrowseInvoices()
{
    if (actInvoiceView != NULL) delete actInvoiceView;
    if (actReminder != NULL) delete actReminder;
    if (actInvoiceDel != NULL) delete actInvoiceDel;
    delete customMenu;
    delete btnStatus;
}


void BrowseInvoices::slotContextMenu()
{
    QMenu m;
    m.addAction(actInvoiceView);
    m.addAction(actInvoiceDel);
    m.addAction(actReminder);
    m.addMenu(customMenu);
    m.exec(QCursor::pos());
}


void BrowseInvoices::slotInvoiceSelected()
{
    GUARD(Protect::FUNC_INV);

    int id = getSelectedItem();
    if (id < 0) return;

    QList<cInvoice *> invoices;
    invoices.append(new cInvoice());
    db.retrieveItem(*invoices.first(), id);
    gendoc.genInvoice(invoices, id);
    delete invoices.takeFirst();
}


void BrowseInvoices::slotInvoiceAdd()
{
    GUARD(Protect::FUNC_INV);
}



void BrowseInvoices::slotInvoiceDel()
{
    GUARD(Protect::FUNC_INV_DEL);

    int id = getSelectedItem();
    if (id < 0) {
        MsgBox::info(QString::fromUtf8("Bitte erst eine Rechnung wählen!"));
        return;
    }

    MsgBox box;
    if (box.execYesNoBox(QString::fromUtf8("Rechnung wirklich löschen?"), MsgBox::BTN_NO) == MsgBox::BTN_YES) {
        if (!db.deleteItem(myDB::TABLE_INVOICES, id))
            MsgBox::error(QString::fromUtf8("Fehler beim Löschen der Rechnung aufgetreten!"));
    }
}


void BrowseInvoices::setStatus(int i)
{
    GUARD(Protect::FUNC_INV);

    int id = getSelectedItem();
    if (id < 0) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Bitte erst eine Rechnung wählen!"), QMessageBox::Ok);
	return;
    }

    cInvoice inv;
    if (db.retrieveItem(inv, id)) {
	inv.setPayStat(i);
	if (db.updateItem(inv)) return;
    }
    QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Der Rechnungsstatus konnte nicht aktualisiert werden!"), QMessageBox::Ok);
}


void BrowseInvoices::slotReminder()
{
    GUARD(Protect::FUNC_REM);

    int id = getSelectedItem();
    if (id < 0) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Bitte erst eine Rechnung wählen!"), QMessageBox::Ok);
	return;
    }

    cInvoice inv;
    if (db.retrieveItem(inv, id) == false) return;

    cReminder reminder;
    reminder.setDate(QDate::currentDate());
    reminder.setInvoiceID(inv.getID());
    reminder.setPatientID(inv.getPatientID());
    reminder.setReceipeID(inv.getReceipeID());
    reminder.setCourseID(inv.getCourseID());
    if (gendoc.genReminder(reminder)) {
	// set current invoice to "angemahnt"
	inv.setPayStat(PAYSTATE_REMINDER);
        if (!db.updateItem(inv))
             QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Der Rechnungsstatus konnte nicht aktualisiert werden!"), QMessageBox::Ok);
    }
}


void BrowseInvoices::slotStatus()
{
    QAction *act = (QAction *)sender();
    int stat = act->data().toInt();
    // TODO: sanity checking
    setStatus(stat);
}
