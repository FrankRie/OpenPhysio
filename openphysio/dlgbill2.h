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

#ifndef DLGBILL2_H
#define DLGBILL2_H

#include <QtWidgets>
#include "ui_dlgbill2.h"
#include "browsereceipes.h"


class DlgBill2 : public QDialog
{
    Q_OBJECT

public:
    enum results { RES_CANCEL = 0, RES_BACK, RES_SAVE };
    DlgBill2(QWidget *parent = 0);
    ~DlgBill2();
    int getResult() { return res; };
    
private:
    Ui::DlgBill2Class ui;
    //TableFilter tf;
    BrowseReceipes *browseReceipes;
    int res;
    
private slots:
    void slotBack();
    void slotCancel();
    void slotSave();
    void slotSetupFilter();
    //void slotUpdate();
    void slotUpdate(int);
    void slotBillSelected();
    void slotBillAll();
};

#endif // DLGBILL2_H
