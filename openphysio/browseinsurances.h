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

#ifndef BROWSEINSURANCES_H
#define BROWSEINSURANCES_H

#include <QtGui>
#include "ui_browseinsurances.h"
#include "browse.h"
#include "mydb.h"


class BrowseInsurances : public Browse
{
    Q_OBJECT
public:
    BrowseInsurances(int mode = MODE_FULL, QWidget *parent = 0);
    ~BrowseInsurances();
    cInsurance& getSelInsurance();
    
private:
    Ui::BrowseInsurancesClass ui;
    cInsurance selInsurance;
    TableFilter *tf;

private slots:
    void slotInsuranceSelected();
    void slotFilter(int i);
};

#endif // BROWSEINSURANCES_H
