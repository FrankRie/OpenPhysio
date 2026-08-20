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

#ifndef BROWSEPINVOICES_H
#define BROWSEPINVOICES_H

#include <QtWidgets>
#include "ui_browsepinvoices.h"
#include "browse.h"
#include "mydb.h"


class BrowsePraxisInvoices : public Browse
{
    Q_OBJECT

public:
    BrowsePraxisInvoices(int mode = MODE_FULL, QWidget *parent = 0);
    ~BrowsePraxisInvoices();

private:
    Ui::BrowsePraxisInvoicesClass ui;
    QAction *actPraxisInvoiceAdd, *actPraxisInvoiceRes, *actPraxisInvoiceView, *actPraxisAppendixView;
    QAction *actStatus, *actReminder;
    QList<QAction *> actions;
    QStringList statnames;
    void setStatus(int i);

private slots:
    void slotContextMenu();
    void slotPraxisInvoiceSelected();
    void slotPraxisAppendixSelected();
    void slotPraxisInvoiceAdd();
    void slotPraxisInvoiceRes();
    void slotReminder();
};

#endif // BROWSEPINVOICES_H
