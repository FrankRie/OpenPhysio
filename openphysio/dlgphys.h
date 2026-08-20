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

#ifndef DLGPHYS_H
#define DLGPHYS_H

#include <QtWidgets>
#include <QMessageBox>
#include "ui_dlgphys.h"
#include "mydb.h"


class DlgPhys : public QDialog
{
    Q_OBJECT

public:
    DlgPhys(QWidget *parent = 0);
    ~DlgPhys();
    void initPhys(cPhys *p);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::DlgPhysClass ui;
    cPhys curPhys;
    QList<cTableItem *> phys_types;
    bool flag_change;
    bool savePhys();
    void checkInput();

private slots:
    void slotSavePhys();
    void slotCancelPhys();
    void slotModified();
    void slotModifiedWithCheck();
    void slotClinic();
    void slotZipCode();

public slots:
    void reject();
};

#endif // DLGPHYS_H
