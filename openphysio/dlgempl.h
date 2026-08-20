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

#ifndef DLGEMPL_H
#define DLGEMPL_H

#include <QtWidgets>
#include <QMessageBox>
#include "ui_dlgempl.h"
#include "mydb.h"

class DlgEmpl : public QDialog
{
    Q_OBJECT

public:
    DlgEmpl(QWidget *parent = 0);
    ~DlgEmpl();
    void initEmployee(cEmployee *e);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::DlgEmplClass ui;
    cEmployee curEmpl;
    QList<cTableItem*> employee_types;
    bool flag_change;
    bool saveEmpl();
    void checkInput();

private slots:
    void slotSaveEmpl();
    void slotCancelEmpl();
    void slotGroup(int i);
    void slotManageTherapyGroup();
    void slotModified();
    void slotModifiedWithCheck();
    void slotZipCode();

public slots:
    void reject();
};

#endif // DLGEMPL_H
