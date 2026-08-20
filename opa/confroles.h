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

#ifndef CONFROLES_H
#define CONFROLES_H

#include "confbase.h"

namespace Ui {
    class ConfRoles;
}

class ConfRoles : public ConfBase {
    Q_OBJECT
    Q_DISABLE_COPY(ConfRoles)
public:
    explicit ConfRoles(QWidget *parent = 0, int l = -1, int a = -1);
    virtual ~ConfRoles();
    void init();
    bool write();

private:
    Ui::ConfRoles *m_ui;
    QList<cTableItem *> roles;
    QList<cTableItem *> funcs;
    QList<int> delRoles;
    void cleanup(bool re_init);

private slots:
    void slotAdd();
    void slotDel();
    void slotEdit();
    void slotDefRoleChanged();
};

#endif // CONFROLES_H
