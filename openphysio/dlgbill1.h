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

#ifndef DLGBILL1_H
#define DLGBILL1_H

#include <QtWidgets>
#include "ui_dlgbill1.h"
#include "browsereceipes.h"


class DlgBill1 : public QDialog
{
    Q_OBJECT

public:
    DlgBill1(QWidget *parent = 0);
    ~DlgBill1();

private:
    Ui::DlgBill1Class ui;
    //TableFilter tf;
    BrowseReceipes *browseReceipes;
    void setupFilter();
    
private slots:
    void slotCancel();
    void slotNext();
    void slotUpdate();
};

#endif // DLGBILL1_H
