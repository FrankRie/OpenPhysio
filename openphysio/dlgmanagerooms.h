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

#ifndef DLGMANAGEROOMS_H
#define DLGMANAGEROOMS_H

#include <QtWidgets>
#include "mydb.h"

namespace Ui {
    class DlgManageRooms;
}

class DlgManageRooms : public QDialog {
    Q_OBJECT
public:
    DlgManageRooms(QWidget *parent = 0);
    ~DlgManageRooms();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::DlgManageRooms *ui;
    QList<cTableItem*> rooms;
    bool flag_change;
    void loadTable();

private slots:
    void slotModified();
    void slotAddRoom();
    void slotDelRoom();
    void slotSave();
    void slotCancel();
};

#endif // DLGMANAGEROOMS_H
