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

#ifndef DLGRECEIPEOPTIONS_H
#define DLGRECEIPEOPTIONS_H

#include <QtWidgets>
#include "ui_dlgreceipeoptions.h"

class DlgReceipeOptions : public QDialog
{
    Q_OBJECT

public:
    DlgReceipeOptions(QWidget *parent = 0);
    ~DlgReceipeOptions();
    void init(bool manCalc);
    bool isManPriceEnabled(int *id);
    bool isManCalcEnabled();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::DlgReceipeOptionsClass ui;
    QList<int> listID;
    QStringList listDescr;

private slots:
    void slotCancel();
    void slotSave();
    void slotManPrice(int);
	
};

#endif // DLGRECEIPEOPTIONS_H
