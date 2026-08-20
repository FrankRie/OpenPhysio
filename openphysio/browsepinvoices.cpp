/****************************************************************************
**
** Copyright (C) 2010 Sebastian Vandersee
**
** This file is part of OpenPhysio.
**
** This file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
****************************************************************************/

#include "browsepinvoices.h"
#include "dlgpinvoices.h"
#include "config.h"
#include "mydb.h"
#include "gendoc.h"
#include "protect.h"

#define PAYSTATE_REMINDER  1


BrowsePraxisInvoices::BrowsePraxisInvoices(int mode, QWidget *parent)
    : Browse(mode, parent)
{
    ui.setupUi(this);
    init((TableViewAnimated*)ui.tvPraxisInvoices, myDB::TABLE_PINVOICES, 1, Qt::DescendingOrder);

    // add display filter widget and toolbar
    QGridLayout *layout = (QGridLayout*) this->layout();
    layout->addWidget(&browseToolBar, 0, 0);
    layout->addWidget(&displayFilter, 0, 1);

    connect(ui.tvPraxisInvoices, SIGNAL(activated(const QModelIndex&)), this, SLOT(slotPraxisInvoiceSelected()));

    actPraxisInvoiceView = new QAction(QIcon(":/edit.png"), QString::fromUtf8("Rechnung ansehen"), this);
    actPraxisAppendixView = new QAction(QIcon(":/finding.png"), QString::fromUtf8("Anlage generieren/ansehen"), this);
    actPraxisInvoiceAdd = new QAction(QIcon(":/new.png"), QString::fromUtf8("Neue Rechnung"), this);
    actPraxisInvoiceRes = new QAction(QIcon(":/report.png"), QString::fromUtf8("Anlage neu bilden"), this);
    actReminder = new QAction(QIcon(":/reminder.png"), QString::fromUtf8("Mahnung erstellen"), this);

    connect(actPraxisInvoiceView, SIGNAL(triggered()), this, SLOT(slotPraxisInvoiceSelected()));
    connect(actPraxisAppendixView, SIGNAL(triggered()), this, SLOT(slotPraxisAppendixSelected()));
    connect(actPraxisInvoiceAdd, SIGNAL(triggered()), this, SLOT(slotPraxisInvoiceAdd()));
    connect(actPraxisInvoiceRes, SIGNAL(triggered()), this, SLOT(slotPraxisInvoiceRes()));
    connect(actReminder, SIGNAL(triggered()), this, SLOT(slotReminder()));

    for (int i = 0; i < db.listInvoiceStates.count(); i++)
        statnames << db.listInvoiceStates.at(i)->getName();

    browseToolBar.addAction(actPraxisInvoiceView);
    browseToolBar.addSeparator();
    browseToolBar.addAction(actPraxisAppendixView);
    browseToolBar.addSeparator();
    browseToolBar.addAction(actPraxisInvoiceAdd);
    browseToolBar.addSeparator();
    browseToolBar.addAction(actReminder);
    ui.tvPraxisInvoices->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui.tvPraxisInvoices, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotContextMenu()));

    if (myConf.isTableColorMode()) {
        ui.tvPraxisInvoices->setAlternatingRowColors(false);
	QStringList keywords;
	QList<QColor> colors;
	keywords << "Offen" << "Angemahnt";
	//colors << QColor(231, 189, 45, 255) << QColor(231, 98, 45, 255);
	colors << myConf.getColor("Offen") << myConf.getColor("Angemahnt");
	setBackgroundKeywords(keywords, colors);
        enableBackgroundColorMode(true, 6);
    }
}


BrowsePraxisInvoices::~BrowsePraxisInvoices()
{
    if (actPraxisInvoiceView != NULL) delete actPraxisInvoiceView;
    if (actReminder != NULL) delete actReminder;
}


void BrowsePraxisInvoices::slotContextMenu()
{
    QMenu m;
    QMenu sub(QString::fromUtf8("Status ändern"));

    for (int i = 0; i < statnames.count(); i++) {
	actions.append(new QAction(statnames.at(i), this));
    }
    sub.addActions(actions);

    m.addAction(actPraxisInvoiceView);
    m.addAction(actPraxisAppendixView);
    m.addAction(actPraxisInvoiceAdd);
    m.addAction(actPraxisInvoiceRes);
    m.addAction(actReminder);
    m.addMenu(&sub);
    QAction *a = m.exec(QCursor::pos());
    if (a != NULL) {
	for (int i = 0; i < actions.count(); i++) {
	    if (a->text() == statnames.at(i)) {
		setStatus(i);
		break;
	    }
	}
    }
    while (!actions.isEmpty()) delete actions.takeFirst();
}


void BrowsePraxisInvoices::slotPraxisInvoiceSelected()
{
    GUARD(Protect::FUNC_INV);

    int id = getSelectedItem();
    if (id < 0) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Bitte erst eine Rechnung wählen!"), QMessageBox::Ok);
        return;
    }

    QList<cPraxisInvoice *> invoices;
    invoices.append(new cPraxisInvoice());
    db.retrieveItem(*invoices.first(), id);
    gendoc.genPraxisInvoice(invoices);
    delete invoices.takeFirst();
}

void BrowsePraxisInvoices::slotPraxisAppendixSelected()
{
    GUARD(Protect::FUNC_INV);

    int id = getSelectedItem();
    if (id < 0) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Bitte erst eine Rechnung wählen!"), QMessageBox::Ok);
        return;
    }

    QList<cPraxisInvoice *> invoices;
    invoices.append(new cPraxisInvoice());
    db.retrieveItem(*invoices.first(), id);
    gendoc.genPraxisAppendix(invoices);
    delete invoices.takeFirst();
}


void BrowsePraxisInvoices::slotPraxisInvoiceAdd()
{
    GUARD(Protect::FUNC_INV);

    DlgPraxisInvoices dlgPraxisInvoices;
    cPraxisInvoice p;

    dlgPraxisInvoices.initPraxisInvoices(&p);
    dlgPraxisInvoices.exec();
}


void BrowsePraxisInvoices::slotPraxisInvoiceRes()
{
    GUARD(Protect::FUNC_INV);

    int id = getSelectedItem();
    if (id < 0) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Bitte erst eine Rechnung wählen!"), QMessageBox::Ok);
        return;
    }

    QList<cPraxisInvoice *> invoices;
    invoices.append(new cPraxisInvoice());
    db.retrieveItem(*invoices.first(), id);
    invoices.first()->setRlist("");
    db.updateItem(*invoices.first(), id);
    gendoc.genPraxisAppendix(invoices);
    delete invoices.takeFirst();
}


void BrowsePraxisInvoices::setStatus(int i)
{
    GUARD(Protect::FUNC_INV);

    int id = getSelectedItem();
    if (id < 0) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Bitte erst eine Rechnung wählen!"), QMessageBox::Ok);
	return;
    }

    cPraxisInvoice inv;
    if (db.retrieveItem(inv, id)) {
	inv.setPayStat(i);
	if (db.updateItem(inv)) return;
    }
    QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Der Rechnungsstatus konnte nicht aktualisiert werden!"), QMessageBox::Ok);
}


void BrowsePraxisInvoices::slotReminder()
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
