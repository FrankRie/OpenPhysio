/****************************************************************************
**
** Copyright (C) 2013 Sebastian Vandersee
** All rights reserved.
**
** This file is part of OpenPhysio.
**
** Please refer to the LICENSE file for terms of use.
**
****************************************************************************/

#ifndef CONFPRINTER_H
#define CONFPRINTER_H

#include "confbase.h"
#include <QtPrintSupport/QPrinterInfo>

namespace Ui {
    class ConfPrinter;
}

class ConfPrinter : public ConfBase {
    Q_OBJECT
    Q_DISABLE_COPY(ConfPrinter)
public:
    ConfPrinter(QWidget *parent = 0, int l = -1, int a = -1);
    virtual ~ConfPrinter();
    void init();
    bool write();

private:
    Ui::ConfPrinter *m_ui;

private slots:
    void slotTestPrint();
    void slotTestPrintDoc();
    void slotTestPrintDates();
};

#endif // CONFPRINTER_H
