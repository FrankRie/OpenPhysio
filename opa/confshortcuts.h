/****************************************************************************
**
** Copyright (C) 2013 Sebastian Vandersee
** All rights reserved.
**
** This file is part of OpenPhysio.
**
** Please refer to the LICENSE file for terms of use.
**
****************************************************************************/

#ifndef CONFSHORTCUTS_H
#define CONFSHORTCUTS_H

#include "confbase.h"

namespace Ui {
    class ConfShortcuts;
}

class ConfShortcuts : public ConfBase {
    Q_OBJECT
public:
    ConfShortcuts(QWidget *parent = 0, int l = -1, int a = -1);
    ~ConfShortcuts();
    void init();
    bool write();

private:
    Ui::ConfShortcuts *ui;
    QList<cTableItem*> listReceipeShortcuts;
    QList<cTableItem*> list_prof;
    int curProf;
    void loadTable(int prof);

private slots:
    void slotAdd();
    void slotDel();
    void slotUp();
    void slotDown();
    void slotModified();
    void slotSelectProfession(int i);
};

#endif // CONFSHORTCUTS_H
