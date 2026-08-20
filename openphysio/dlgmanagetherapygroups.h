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

#ifndef DLGMANAGETHERAPYGROUPS_H
#define DLGMANAGETHERAPYGROUPS_H

#include <QtWidgets>
#include "mydb.h"

namespace Ui {
    class DlgManageTherapyGroups;
}

class DlgManageTherapyGroups : public QDialog {
    Q_OBJECT
public:
    DlgManageTherapyGroups(QWidget *parent = 0);
    ~DlgManageTherapyGroups();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::DlgManageTherapyGroups *ui;
    QList<cTableItem*> groups;
    bool flag_change;
    void loadTable();

private slots:
    void slotModified();
    void slotAddGroup();
    void slotDelGroup();
    void slotSave();
    void slotCancel();
};

#endif // DLGMANAGETHERAPYGROUPS_H
