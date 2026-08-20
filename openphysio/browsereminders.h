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

#ifndef BROWSEREMINDERS_H
#define BROWSEREMINDERS_H

#include <QtGui>
#include "ui_browsereminders.h"
#include "browse.h"

namespace Ui {
    class BrowseReminders;
}

class BrowseReminders : public Browse {
    Q_OBJECT
    Q_DISABLE_COPY(BrowseReminders)
public:
    explicit BrowseReminders(int mode = MODE_FULL, QWidget *parent = 0);
    virtual ~BrowseReminders();

private:
    TableViewAnimated *tvReminders;
    QAction *actReminderView, *actReminderDel;
    Ui::BrowseReminders *m_ui;

private slots:
    void slotReminderSelected();
    void slotReminderDel();
};

#endif // BROWSEREMINDERS_H
