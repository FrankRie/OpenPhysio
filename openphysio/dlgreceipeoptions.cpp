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

#include "dlgreceipeoptions.h"
#include "mydb.h"
#include "helperwrapper.h"


DlgReceipeOptions::DlgReceipeOptions(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    if (db.getAvailablePriceTablesForGKV(listID, listDescr)) {
	ui.cbPriceList->addItems(listDescr);
    }
    ui.cbManPrice->setChecked(false);
    ui.cbPriceList->setEnabled(false);
	
    connect(ui.pbCancel, &QPushButton::clicked, this, &DlgReceipeOptions::slotCancel);
    connect(ui.pbSave, &QPushButton::clicked, this, &DlgReceipeOptions::slotSave);
    connect(ui.cbManPrice, &QCheckBox::checkStateChanged, this, &DlgReceipeOptions::slotManPrice);
    installEventFilter(this);
}


DlgReceipeOptions::~DlgReceipeOptions()
{
}


void DlgReceipeOptions::init(bool manCalc)
{
    ui.cbManCalc->setChecked(manCalc);
}


void DlgReceipeOptions::slotCancel()
{
    this->reject();
}


void DlgReceipeOptions::slotSave()
{
    this->accept();
}


void DlgReceipeOptions::slotManPrice(int)
{
    if (ui.cbManPrice->isChecked()) ui.cbPriceList->setEnabled(true);
    else ui.cbPriceList->setEnabled(false);
}


bool DlgReceipeOptions::isManPriceEnabled(int *id)
{
    if (ui.cbManPrice->isChecked()) {
	int tmp = ui.cbPriceList->currentIndex();
	if (tmp >= 0 && tmp < listID.count()) {
	    *id = listID.at(tmp);
	} else {
	    *id = -1;
	}
	return true;
    } else {
	*id = -1;
	return false;
    }
}


bool DlgReceipeOptions::isManCalcEnabled()
{
    if (ui.cbManCalc->isChecked()) {
	return true;
    } else {
	return false;
    }
}


bool DlgReceipeOptions::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        if (ke->key() == Qt::Key_F1 || ke->key() == Qt::Key_Help) {
            helpWrapper.showHelpForKeyword("openphysio_receipe_options", true);
            return true;
        }
    }
    return QDialog::eventFilter(obj, event);
}
