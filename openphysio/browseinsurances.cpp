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

#include "browseinsurances.h"
#include "mydb.h"
#include "config.h"


BrowseInsurances::BrowseInsurances(int mode, QWidget *parent)
    : Browse(mode, parent)
{
    ui.setupUi(this);
    displayFilter.useAutoKeyboardFocus(true);
    init((TableViewAnimated*)ui.tvInsurances, myDB::TABLE_INSURANCES);

    for (int i = 0; i < db.listAssociations.count(); i++)
        ui.cbFilter->addItem(db.listAssociations.at(i)->getNameShort());
    connect(ui.cbFilter, &QComboBox::currentIndexChanged, this, &BrowseInsurances::slotFilter);

    if (mode == MODE_SELECT) {
    connect(ui.tvInsurances, &QTableView::activated, this, &BrowseInsurances::slotInsuranceSelected);
    }

    tf = new TableFilter(myDB::TABLE_INSURANCES);
    if (myConf.usePreload()) slotFilter(0);

    // add display filter widget and toolbar
    QGridLayout *layout = (QGridLayout*) this->layout();
    layout->addWidget(&displayFilter, 0, 2);
}


BrowseInsurances::~BrowseInsurances()
{
    if (tf != NULL) delete tf;
}


void BrowseInsurances::slotInsuranceSelected()
{
    int id = getSelectedItem();
    if (id < 0) return;

    db.retrieveItem(selInsurance, id);
    emit sigItemSelected();
}


cInsurance& BrowseInsurances::getSelInsurance()
{
    return selInsurance;
}


void BrowseInsurances::slotFilter(int i)
{
    if (i < 0) return;
    tf->clear();
    if (i == 0) tf->addFilterExpr(false, TableFilter::F_INSURANCE_AID, TableFilter::OP_NOTEQUAL, 0);
    else tf->addFilterExpr(false, TableFilter::F_INSURANCE_AID, TableFilter::OP_EQUAL, i);
    this->setFilter(tf);

//    if (i == 0) this->setFilter(-1, 0, Browse::OP_NOTEQUAL);
//    else this->setFilter(Browse::F_INSURANCE_AID, i, Browse::OP_EQUAL);
}
