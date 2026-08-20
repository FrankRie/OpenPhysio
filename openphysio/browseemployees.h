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

#ifndef BROWSEEMPLOYEES_H
#define BROWSEEMPLOYEES_H


#include <QtWidgets>
#include "ui_browseemployees.h"
#include "browse.h"
#include "dlgempl.h"


class BrowseEmployees : public Browse
{
    Q_OBJECT
public:
    BrowseEmployees(int mode = MODE_FULL, QWidget *parent = 0);
    ~BrowseEmployees();

private:
    Ui::BrowseEmployeesClass ui;
    QAction *actEmplAdd, *actEmplEdit, *actEmplDel;
    
public slots:
    void slotEmplAdd();

private slots:
    void slotEmplEdit();
    void slotEmplDel();
};


#endif // BROWSEEMPLOYEES_H
