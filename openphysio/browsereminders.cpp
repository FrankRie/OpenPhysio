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

#include "browsereminders.h"
#include "gendoc.h"
#include "mydb.h"
#include "protect.h"
#include "msgbox.h"


BrowseReminders::BrowseReminders(int mode, QWidget *parent) :
    Browse(mode, parent),
    m_ui(new Ui::BrowseReminders)
{
    m_ui->setupUi(this);

    tvReminders = new TableViewAnimated(this);
    tvReminders->setObjectName(QString::fromUtf8("tvReminders"));
    tvReminders->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    tvReminders->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    tvReminders->setAlternatingRowColors(true);
    tvReminders->setSelectionMode(QAbstractItemView::SingleSelection);
    tvReminders->setSelectionBehavior(QAbstractItemView::SelectRows);
    tvReminders->setSortingEnabled(true);

    tvReminders->setStyleSheet(
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

    m_ui->gridLayout->addWidget(tvReminders, 1, 0, 1, 6);

    init(tvReminders, myDB::TABLE_REMINDERS, 1, Qt::DescendingOrder);

    // add display filter widget and toolbar
    QGridLayout *layout = (QGridLayout*) this->layout();
    layout->addWidget(&browseToolBar, 0, 0);
    layout->addWidget(&displayFilter, 0, 1);

    actReminderView = new QAction(QIcon(":/edit.png"), QString::fromUtf8("Mahnung ansehen"), this);
    actReminderDel = new QAction(QIcon(":/delete.png"), QString::fromUtf8("Mahnung löschen"), this);
    connect(actReminderView, SIGNAL(triggered()), this, SLOT(slotReminderSelected()));
    connect(actReminderDel, SIGNAL(triggered()), this, SLOT(slotReminderDel()));
    browseToolBar.addAction(actReminderView);
    browseToolBar.addAction(actReminderDel);
    tvReminders->addAction(actReminderView);
    tvReminders->addAction(actReminderDel);
    tvReminders->setContextMenuPolicy(Qt::ActionsContextMenu);

    connect(tvReminders, SIGNAL(activated(const QModelIndex&)), this, SLOT(slotReminderSelected()));
}


BrowseReminders::~BrowseReminders()
{
    if (actReminderView != NULL) delete actReminderView;
    if (actReminderDel != NULL) delete actReminderDel;
    delete m_ui;
    delete tvReminders;
}


void BrowseReminders::slotReminderSelected()
{
    GUARD(Protect::FUNC_REM);

    int id = getSelectedItem();
    if (id < 0) {
        MsgBox::info(QString::fromUtf8("Bitte erst eine Mahnung auswählen!"));
        return;
    }

    cReminder reminder;
    if (db.retrieveItem(reminder, id)) gendoc.genReminder(reminder);
}


void BrowseReminders::slotReminderDel()
{
    GUARD(Protect::FUNC_REM_DEL);

    int id = getSelectedItem();
    if (id < 0) {
        MsgBox::info(QString::fromUtf8("Bitte erst eine Mahnung wählen!"));
        return;
    }

    MsgBox box;
    if (box.execYesNoBox(QString::fromUtf8("Mahnung wirklich löschen?"), MsgBox::BTN_NO) == MsgBox::BTN_YES) {
        if (!db.deleteItem(myDB::TABLE_REMINDERS, id))
            MsgBox::error(QString::fromUtf8("Fehler beim Löschen der Mahnung aufgetreten!"));
    }
}
