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

#ifndef DLGMANAGECARS_H
#define DLGMANAGECARS_H

#include <QtWidgets>
#include "mydb.h"


namespace Ui {
    class DlgManageCars;
}

class DlgManageCars : public QDialog {
    Q_OBJECT
public:
    DlgManageCars(QWidget *parent = 0);
    ~DlgManageCars();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::DlgManageCars *ui;
    QList<cTableItem*> cars;
    bool flag_change;
    void loadTable();

private slots:
    void slotModified();
    void slotAddCar();
    void slotDelCar();
    void slotSave();
    void slotCancel();

};

#endif // DLGMANAGECARS_H
