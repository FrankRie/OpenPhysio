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

#ifndef BROWSEINVOICES_H
#define BROWSEINVOICES_H

#include <QtWidgets>
#include "ui_browseinvoices.h"
#include "browse.h"
#include "mydb.h"


class BrowseInvoices : public Browse
{
    Q_OBJECT
public:
    BrowseInvoices(int mode = MODE_FULL, QWidget *parent = 0);
    ~BrowseInvoices();

private:
    Ui::BrowseInvoicesClass ui;
    QMenu *customMenu;
    QToolButton *btnStatus;
    QAction *actInvoiceAdd, *actInvoiceDel, *actInvoiceView;
    QAction *actReminder;
    void setStatus(int i);

private slots:
    void slotContextMenu();
    void slotInvoiceSelected();
    void slotInvoiceAdd();
    void slotInvoiceDel();
    void slotReminder();
    void slotStatus();
};

#endif // BROWSEINVOICES_H
